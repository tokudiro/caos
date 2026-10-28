/*
 * Prolog of the lexer: globals and helpers shared by the *.l rules.
 * Not compiled on its own; the Makefile pastes it into obj/caos.l.
 * The Caos class itself is written in Caos.caos.
 */
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


int bracket = 0;
boolean arg_analysis = FALSE;
boolean arg_comma = FALSE;
boolean isMember = FALSE;

SBuf* class_buf;
SBuf* thispointer_buf;
char allque[MAX_TEXT*MAX_ARRAYSIZE];
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
