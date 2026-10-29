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

/*
 * Pushed input : the text of an expanded template, or of a contract, is scanned
 * in place of the file and then the scanner goes on where it was.
 */
typedef struct {
    struct yy_buffer_state* buffer;
    int line;
    char* file;
    int state;
    boolean isInjection;
} PushedInput;
static PushedInput pushedInput[4];
static int pushedCount = 0;
static int injectDepth = 0;     /* > 0 while the text of a contract is scanned */

#define PUSH_INPUT(text, newLine, newFile, resumeState, injection) do { \
    if (pushedCount >= 4) { \
        reportError("too many nested expansions"); \
    } else { \
        pushedInput[pushedCount].buffer = YY_CURRENT_BUFFER; \
        pushedInput[pushedCount].line = yylineno; \
        pushedInput[pushedCount].file = input_filename; \
        pushedInput[pushedCount].state = (resumeState); \
        pushedInput[pushedCount].isInjection = (injection); \
        pushedCount++; \
        if (injection) injectDepth++; \
        yy_scan_string(text); \
        yylineno = (newLine); \
        input_filename = (char*)(newFile); \
    } \
} while (0)

#define POP_INPUT() do { \
    PushedInput* top_ = &pushedInput[--pushedCount]; \
    yy_delete_buffer(YY_CURRENT_BUFFER); \
    yy_switch_to_buffer(top_->buffer); \
    yylineno = top_->line; \
    input_filename = top_->file; \
    if (top_->isInjection) injectDepth--; \
    BEGIN(top_->state); \
} while (0)

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

/*
 * Contracts : "#require expr" and "#ensure expr" between the header of a method
 * and its '{'. They are collected as C text, and scanned as if they were written
 * in the body (so that @member and x->method() work).
 */
static char contractRequire[MAX_TEXTLINE];
static size_t contractRequireLen = 0;
static char contractEnsure[MAX_TEXTLINE];
static size_t contractEnsureLen = 0;
static boolean isContractIncluded = FALSE;

/* of the method being translated */
static char methodEnsureReturn[2 * MAX_TEXTLINE + 64];   /* after "return x;" */
static char methodEnsureEnd[MAX_TEXTLINE + 8];      /* before the last '}' of a void method */
static boolean methodHasEnsure = FALSE;
static boolean methodIsVoid = FALSE;
static boolean methodEndDone = FALSE;
static boolean methodInReturn = FALSE;

static void addContract(boolean isRequire, const char* expr)
{
    char one[MAX_TEXTLINE];
    size_t n;
    while (*expr == ' ' || *expr == '\t') expr++;
    n = strlen(expr);
    while (n > 0 && (expr[n-1] == ' ' || expr[n-1] == '\t' || expr[n-1] == '\r')) n--;
    if (n == 0) {
        reportError(isRequire ? "#require needs an expression" : "#ensure needs an expression");
        return;
    }
    if (!isContractIncluded) {
        /* before the header of the method, which is still queued */
        fprintf(source, "#include \"caos_contract.h\"\n");
        if (isLineNo) fprintf(source, "#line %d \"%s\"\n", methodLine, input_filename);
        isContractIncluded = TRUE;
    }
    if (n + 32 >= sizeof(one)) {
        reportError("the line is too long");
        return;
    }
    snprintf(one, sizeof(one), "%s(%.*s);", isRequire ? "CAOS_REQUIRE" : "CAOS_ENSURE", (int)n, expr);
    /* the texts are joined with a blank */
    if (isRequire) {
        if (contractRequireLen > 0) appendLimited(contractRequire, &contractRequireLen, " ");
        appendLimited(contractRequire, &contractRequireLen, one);
    } else {
        if (contractEnsureLen > 0) appendLimited(contractEnsure, &contractEnsureLen, " ");
        appendLimited(contractEnsure, &contractEnsureLen, one);
    }
}

/*
 * At the '{' of a method : the text to scan first (the declaration of result
 * and the preconditions), and the texts kept for the postconditions.
 */
static void startContracts(const char* proto, char* head, size_t size)
{
    char ret[MAX_TEXT];
    head[0] = 0;
    methodHasEnsure = (contractEnsureLen > 0);
    methodEndDone = FALSE;
    methodInReturn = FALSE;
    Iface_returnType(proto, ret, sizeof(ret));
    methodIsVoid = (strcmp(ret, "void") == 0);
    if (methodHasEnsure && !methodIsVoid && ret[0] != 0) {
        snprintf(head, size, " %s result;", ret);
    }
    if (contractRequireLen > 0) {
        if (strlen(head) + contractRequireLen + 1 < size) {
            strcat(head, " ");
            strcat(head, contractRequire);
        }
    }
    snprintf(methodEnsureEnd, sizeof(methodEnsureEnd), " %s ", contractEnsure);
    snprintf(methodEnsureReturn, sizeof(methodEnsureReturn), " %s %s", contractEnsure,
             methodIsVoid ? "return; }" : "return result; }");
    contractRequire[0] = 0;
    contractRequireLen = 0;
    contractEnsure[0] = 0;
    contractEnsureLen = 0;
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
