#include <stdio.h>
#include <string.h>

#include "contract.h"
#include "proto.h"
#include "str.h"

ContractMethod contractMethod;

/* a collected text : the macros of the expressions joined with a blank */
typedef struct {
    char text[CONTRACT_LINE];
    size_t len;
} Collected;

static Collected require;
static Collected ensure;
static Collected invariantEntry;    /* CAOS_REQUIRE(...) of the invariant */
static Collected invariantExit;     /* CAOS_ENSURE(...) of the invariant */
static int isNoInvariant = 0;

static int appendTo(Collected* c, const char* str)
{
    return Str_append(c->text, &c->len, sizeof(c->text), str);
}

static int appendMacro(Collected* c, const char* macro, const char* expr, size_t n)
{
    char one[CONTRACT_LINE];
    if (n + 32 >= sizeof(one)) return 0;
    snprintf(one, sizeof(one), "%s(%.*s);", macro, (int)n, expr);
    return (c->len == 0 || appendTo(c, " ")) && appendTo(c, one);
}

static void clear(Collected* c)
{
    c->text[0] = 0;
    c->len = 0;
}

const char* Contract_add(ContractKind kind, const char* expr)
{
    static const char* names[] = { "#require", "#ensure", "#invariant" };
    static char message[64];
    size_t n;
    int ok;

    while (*expr == ' ' || *expr == '\t') expr++;
    n = strlen(expr);
    while (n > 0 && (expr[n-1] == ' ' || expr[n-1] == '\t' || expr[n-1] == '\r')) n--;
    if (n == 0) {
        snprintf(message, sizeof(message), "%s needs an expression", names[kind]);
        return message;
    }
    switch (kind) {
    case CONTRACT_REQUIRE:
        ok = appendMacro(&require, "CAOS_REQUIRE", expr, n);
        break;
    case CONTRACT_ENSURE:
        ok = appendMacro(&ensure, "CAOS_ENSURE", expr, n);
        break;
    default:
        ok = appendMacro(&invariantEntry, "CAOS_REQUIRE", expr, n)
          && appendMacro(&invariantExit, "CAOS_ENSURE", expr, n);
        break;
    }
    return ok ? 0 : "the line is too long";
}

void Contract_resetClass(void)
{
    clear(&invariantEntry);
    clear(&invariantExit);
}

void Contract_noInvariant(void)
{
    isNoInvariant = 1;
}

int Contract_hasInvariant(visibility vis, scope sc)
{
    return invariantEntry.len > 0 && vis == VISIBILITY_PUBLIC && sc == SCOPE_OBJECT;
}

/* text + blank + more, either may be empty */
static void join(char* dst, size_t size, const char* a, const char* b)
{
    snprintf(dst, size, "%s%s%s", a, (a[0] && b[0]) ? " " : "", b);
}

void Contract_start(const char* proto, visibility vis, scope sc, mutability mut, char* head, size_t size)
{
    char ret[MAX_TEXT];
    char combined[2 * CONTRACT_LINE + 8];
    int entry = Contract_hasInvariant(vis, sc);
    int exitCheck = entry && mut == MUTABILITY_MUTABLE;

    head[0] = 0;
    contractMethod.hasEnsure = (ensure.len > 0 || exitCheck);
    contractMethod.endDone = 0;
    contractMethod.inReturn = 0;
    Proto_returnType(proto, ret, sizeof(ret));
    contractMethod.isVoid = (strcmp(ret, "void") == 0);
    if (contractMethod.hasEnsure && !contractMethod.isVoid && ret[0] != 0) {
        snprintf(head, size, " %s result;", ret);
    }
    if (entry && !isNoInvariant && strlen(head) + invariantEntry.len + 1 < size) {
        strcat(head, " ");
        strcat(head, invariantEntry.text);
    }
    if (require.len > 0 && strlen(head) + require.len + 1 < size) {
        strcat(head, " ");
        strcat(head, require.text);
    }

    /* the postconditions, then the invariant */
    join(combined, sizeof(combined), ensure.text, exitCheck ? invariantExit.text : "");
    snprintf(contractMethod.endText, sizeof(contractMethod.endText), " %s ", combined);
    snprintf(contractMethod.returnText, sizeof(contractMethod.returnText), " %s %s", combined,
             contractMethod.isVoid ? "return; }" : "return result; }");

    clear(&require);
    clear(&ensure);
    isNoInvariant = 0;
}
