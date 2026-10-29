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
#include "contract.h"
#include "iface.h"
#include "str.h"
#include "template.h"
#include "vars.h"


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
    if (!Str_append(buf, len, MAX_TEXTLINE, str)) {
        reportError("the line is too long");
    }
}

static void finishTemplate(const char* endDirective)
{
    templateHead[templateHeadLen] = 0;
    Template_define(templateName, templateHead, classDirective, endDirective,
                    templateLine, input_filename, ifaceError);
}

/* "var->method(" and the like : see Vars_resolveCall */
static int resolveCall(const char* text, boolean isMember, char* call, size_t size, size_t* keep)
{
    const char* error;
    int found = Vars_resolveCall(text, isMember, SBuf_getStr(thispointer_buf), call, size, keep, &error);
    if (error) reportError(error);
    return found;
}

/*
 * Contracts : "#require expr", "#ensure expr" and "#invariant expr". The text
 * is built by contract.c ; the include of the macros goes before the header of
 * the method, which is still queued.
 */
static boolean isContractIncluded = FALSE;

static void includeContract(void)
{
    if (!isContractIncluded) {
        fprintf(source, "#include \"caos_contract.h\"\n");
        if (isLineNo) fprintf(source, "#line %d \"%s\"\n", methodLine, input_filename);
        isContractIncluded = TRUE;
    }
}

static void addContract(ContractKind kind, const char* expr)
{
    const char* error = Contract_add(kind, expr);
    if (error) {
        reportError(error);
    } else if (kind != CONTRACT_INVARIANT) {
        includeContract();
    }
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
