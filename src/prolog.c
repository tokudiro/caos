/*
 * Prolog of the lexer: globals and helpers shared by the *.l rules.
 * Not compiled on its own; the Makefile pastes it into obj/caos.l.
 * The Caos class itself is written in Caos.caos.
 */
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"


#include "RBuf.h"
#include "SBuf.h"
#include "SLib.h"

#include "Caos.h"
#include "global.h"
#include "ListBuf.h"
#include "Element.h"
#include "iface.h"
#include "template.h"


int bracket = 0;
boolean arg_analysis = FALSE;
boolean arg_comma = FALSE;
boolean isMember = FALSE;

SBuf* class_buf;
SBuf* thispointer_buf;
char allque[MAX_ALLQUE];
char* input_filename;
char filename[MAX_TEXT];
char source_name[MAX_TEXT];
char public_header_name[MAX_TEXT];
char private_header_name[MAX_TEXT];
char define_header_name[MAX_TEXT];

RBuf* queue;
Caos* caos;

boolean isLineNo = TRUE;
boolean isVerbose = FALSE;
boolean isOldComment = FALSE;
boolean isNonPrivateHeader = FALSE;
boolean isNonDefineHeader = FALSE;
boolean isOutputHeader = FALSE;
/* the source already includes the headers of a class / the public header */
boolean isSourceIncluded = FALSE;
boolean isPublicIncluded = FALSE;

/*
 * A comment inside a declaration that is still queued is queued with it,
 * so that it is output together with the declaration.
 */
static boolean isCommentQueued = FALSE;
static char queued_comment[MAX_TEXT];

static boolean isDeclarationQueued(const RBuf* q)
{
    return (RBuf_isEmpty(q) == FALSE && RBuf_isWSPACE((RBuf*)q) == FALSE);
}

static void queued_comment_flush(void)
{
    if (queued_comment[0] != 0) {
        RBuf_enque(queue, queued_comment, strlen(queued_comment), T_NON);
        queued_comment[0] = 0;
    }
}

static void queued_comment_put(const char* str)
{
    /* one queue element holds MAX_TEXT chars : split a long comment */
    if (strlen(queued_comment) + strlen(str) >= MAX_TEXT) {
        queued_comment_flush();
    }
    strcat(queued_comment, str);
}

visibility methodVisibility = VISIBILITY_NONE;
scope methodScope = SCOPE_OBJECT;
mutability methodMutability = MUTABILITY_MUTABLE;
classtype classType = TYPE_CLASS;

int countPublicMethod = 0;
int countPrivateMethod = 0;
int countClassMethod = 0;
int countAttribute = 0;

/* errors found while translating : main() exits with non-zero status */
int errorCount = 0;

/* line where the current class starts (for "#endclass is missing") */
static int classLine = 0;

/* line where the current method starts (for "} is missing") */
static int methodLine = 0;

static void reportErrorAt(int line, const char* message)
{
    fprintf(stderr, "%s:%d: error: %s\n", input_filename, line, message);
    errorCount++;
}

static void reportError(const char* message)
{
    reportErrorAt(yylineno, message);
}

static void ifaceError(const char* file, int line, const char* message)
{
    fprintf(stderr, "%s:%d: error: %s\n", file, line, message);
    errorCount++;
}

/* line where the current #interface starts, and its name */
static int interfaceLine = 0;
static char interfaceName[MAX_TEXT];

static void finishInterface(void)
{
    Iface* iface = Iface_define(interfaceName, input_filename, interfaceLine, ifaceError);
    if (iface == 0) return;
    if (!isPublicIncluded) {
        fprintf(source, "#include \"%s\"\n", public_header_name);
        isPublicIncluded = TRUE;
    }
    if (isLineNo) fprintf(public_header, "#line %d \"%s\"\n", interfaceLine, input_filename);
    Iface_emitHeader(iface, public_header);
    if (isLineNo) fprintf(source, "#line %d \"%s\"\n", interfaceLine, input_filename);
    Iface_emitSource(iface, source);
    if (isLineNo) fprintf(source, "#line %d \"%s\"\n", yylineno + 1, input_filename);
}

/* "implements $Name" : the interface comes from this file or from Name.caos */
static void implementsInterface(const char* name)
{
    Iface* iface = Iface_find(name, input_filename, yylineno, ifaceError);
    if (iface == 0) return;
    Iface_classImplements(iface);
    if (iface->path) {
        char header[MAX_TEXT + 4];
        snprintf(header, sizeof(header), "%s.h", name);
        fprintf(public_header, "#include \"%s\"\n", header);
        addImport(header);
        addSourceDep(iface->path);
    }
}

/* class templates : "#class $List<T>" is kept, "#instance" scans the expanded text */
static char classDirective[32];
static int templateLine = 0;
static char templateName[MAX_TEXT];
static char templateHead[MAX_TEXTLINE];
static size_t templateHeadLen = 0;
static char instanceText[MAX_TEXTLINE];
static size_t instanceLen = 0;
static boolean isExpanding = FALSE;
static int savedLine = 0;
static char* savedFile = 0;
static struct yy_buffer_state* savedBuffer = 0;

static void appendLimited(char* buf, size_t* len, const char* str)
{
    size_t n = strlen(str);
    if (*len + n >= MAX_TEXTLINE) {
        reportError("the line is too long");
        return;
    }
    memcpy(buf + *len, str, n);
    *len += n;
    buf[*len] = 0;
}

static void finishTemplate(const char* endDirective)
{
    templateHead[templateHeadLen] = 0;
    Template_define(templateName, templateHead, classDirective, endDirective,
                    templateLine, input_filename, ifaceError);
}

/*
 * "var->method(" / "v.method(" / "@member->method(" : the call "Class_method(arg"
 * for the class of the variable. 0 if the variable is not of a class (nothing is
 * changed then) ; *keep is the length of the variable at the start of the text.
 */
static int resolveCall(const char* text, boolean isMember, char* call, size_t size, size_t* keep)
{
    const char* p = text + (isMember ? 1 : 0);
    char name[MAX_TEXT], method[MAX_TEXT];
    size_t n = 0;
    VarInfo v;
    boolean arrow;
    const char* thisName = SBuf_getStr(thispointer_buf);
    char arg[2 * MAX_TEXT + 8];

    while ((isalnum((unsigned char)*p) || *p == '_') && n < sizeof(name) - 1) name[n++] = *p++;
    name[n] = 0;
    *keep = (size_t)(p - text);
    while (*p == ' ' || *p == '\t') p++;
    arrow = (*p == '-');
    p += arrow ? 2 : 1;
    while (*p == ' ' || *p == '\t') p++;
    n = 0;
    while ((isalnum((unsigned char)*p) || *p == '_') && n < sizeof(method) - 1) method[n++] = *p++;
    method[n] = 0;

    if (!(isMember ? Vars_findMember(name, &v) : Vars_find(name, &v))) return 0;
    if (isMember) snprintf(arg, sizeof(arg), "%s->%s", thisName, name);
    else snprintf(arg, sizeof(arg), "%s", name);

    if (v.isInterface) {
        /* an interface is a pair of pointers, passed by value */
        if (arrow) snprintf(call, size, "%s_%s(*%s", v.cls, method, arg);
        else snprintf(call, size, "%s_%s(%s", v.cls, method, arg);
    } else if (v.stars == 1) {
        if (!arrow) reportError("a pointer calls a method with '->'");
        snprintf(call, size, "%s_%s(%s", v.cls, method, arg);
    } else {
        if (arrow) reportError("a value calls a method with '.'");
        snprintf(call, size, "%s_%s(&%s", v.cls, method, arg);
    }
    return 1;
}

/* #endclass : check "implements" and write the function pointer tables */
static void finishClass(void)
{
    if (Iface_classImplementsCount() > 0) {
        if (isLineNo) fprintf(source, "#line %d \"%s\"\n", classLine, input_filename);
        Iface_classFinish(SBuf_getStr(class_buf), input_filename, classLine, source, public_header, ifaceError);
    }
    Iface_classBegin();
}

/*
 * Run before every rule : a token that does not fit a queue element, or a
 * statement with more tokens than the queue holds, is reported (the queue
 * keeps its content and cuts the rest, so nothing is overwritten).
 */
static void checkLimits(void)
{
    static int overflowLine = -1;
    if (yyleng >= MAX_TEXT) {
        reportError("token is too long (up to 255 characters)");
    }
    if (RBuf_popOverflow(queue) && overflowLine != yylineno) {
        /* once per line : every further token of the statement overflows too */
        overflowLine = yylineno;
        reportError("too many tokens in one statement (up to 254)");
    }
}
#define YY_USER_ACTION checkLimits();
