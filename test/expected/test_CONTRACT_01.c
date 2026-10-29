#include <assert.h>

/* count the violations instead of stopping : caos_contract.h keeps these macros */
static int requireFailed = 0;
static int ensureFailed = 0;
#define CAOS_REQUIRE(cond) do { if (!(cond)) requireFailed++; } while (0)
#define CAOS_ENSURE(cond) do { if (!(cond)) ensureFailed++; } while (0)

#include "test_CONTRACT_01.h"
#include "test_CONTRACT_01_define.h"
#include "test_CONTRACT_01_private.h"


void Stack_init(Stack* pthis)
{
    pthis->length = 0;
}

#include "caos_contract.h"
void Stack_push(Stack* pthis, int v)



{ CAOS_REQUIRE(pthis->length < 4); CAOS_REQUIRE(v >= 0);
    pthis->items[pthis->length++] = v;
 CAOS_ENSURE(pthis->length > 0); }

int Stack_pop(Stack* pthis)


{ int result; CAOS_REQUIRE(pthis->length > 0);
    int v;
    if (pthis->length == 0) { result = 0; CAOS_ENSURE(result >= 0); return result; }
    pthis->length--;
    v = pthis->items[pthis->length];
    { result = v > 50 ? v - 100 : v; CAOS_ENSURE(result >= 0); return result; }
}

int Stack_size(Stack* pthis)
{
    return pthis->length;
}

int Stack_doubled(Stack* pthis, int v)

{ int result;
    { result = v * 2 + (v > 100); CAOS_ENSURE(result == v * 2); return result; }
}

int Stack_dbl(Stack* pthis, int v)
{
    return Stack_doubled(pthis, v);
}

int Stack_twice(int v)


{ int result; CAOS_REQUIRE(v < 1000);
    { result = v * 2; CAOS_ENSURE(result == v * 2); return result; }
}

void Stack_check(Stack* pthis)

{
    pthis->length += 0;
 CAOS_ENSURE(pthis->length >= 0); }


int main(void)
{
    Stack s;
    Stack_init(&s);
    Stack_push(&s, 3);
    Stack_push(&s, 60);
    assert(requireFailed == 0 && ensureFailed == 0);
    assert(Stack_pop(&s) == 60 - 100);          /* result >= 0 is violated */
    assert(ensureFailed == 1);
    assert(Stack_pop(&s) == 3);
    assert(ensureFailed == 1);
    assert(Stack_pop(&s) == 0);                 /* @length > 0 is violated (empty) */
    assert(requireFailed == 1);
    Stack_push(&s, -1);                         /* v >= 0 is violated */
    assert(requireFailed == 2);
    assert(Stack_twice(4) == 8);
    assert(Stack_twice(5000) == 10000 && requireFailed == 3);
    assert(Stack_dbl(&s, 5) == 10 && ensureFailed == 1);
    assert(Stack_dbl(&s, 200) == 401 && ensureFailed == 2);   /* a private method */
    Stack_check(&s);
    assert(ensureFailed == 2);
    return 0;
}
