#ifndef CONTRACT_H__
#define CONTRACT_H__
/*
 * Contracts : "#require", "#ensure" and "#invariant" are collected as the C
 * text of CAOS_REQUIRE(...) / CAOS_ENSURE(...), which the translator scans as if
 * it was written in the body of the method.  Written in C (not in caos): it is
 * a part of the translator only.  See doc/language.md.
 */
#include <stddef.h>

#include "common.h"

#define CONTRACT_LINE MAX_TEXTLINE

typedef enum { CONTRACT_REQUIRE, CONTRACT_ENSURE, CONTRACT_INVARIANT } ContractKind;

/* the method being translated (set by Contract_start, used while its body is scanned) */
typedef struct {
    int hasEnsure;      /* the exit has something to check (#ensure or the invariant) */
    int isVoid;
    int endDone;        /* the check before the last '}' has been written */
    int inReturn;       /* between "return" and its ';' */
    char returnText[3 * CONTRACT_LINE + 64];    /* to scan after "return x;" */
    char endText[3 * CONTRACT_LINE + 64];       /* to scan before the last '}' of a void method */
} ContractMethod;
extern ContractMethod contractMethod;

/* add the expression of a directive : 0, or the message of the error */
const char* Contract_add(ContractKind kind, const char* expr);
/* a new class : forget its invariant */
void Contract_resetClass(void);
/* #noinvariant : the next method has no check on entry */
void Contract_noInvariant(void);
/* does the invariant of the class apply to a method ? */
int Contract_hasInvariant(visibility vis, scope sc);
/*
 * At the '{' of a method : head gets the text to scan first (the declaration of
 * result, the invariant and the preconditions) ; the postconditions are kept in
 * contractMethod. The collected preconditions and postconditions are cleared.
 */
void Contract_start(const char* proto, visibility vis, scope sc, mutability mut, char* head, size_t size);

#endif /* CONTRACT_H__ */
