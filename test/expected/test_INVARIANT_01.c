#include <assert.h>

/* count the violations instead of stopping */
static int entryFailed = 0;
static int exitFailed = 0;
#define CAOS_REQUIRE(cond) do { if (!(cond)) entryFailed++; } while (0)
#define CAOS_ENSURE(cond) do { if (!(cond)) exitFailed++; } while (0)

#include "test_INVARIANT_01.h"
#include "test_INVARIANT_01_define.h"
#include "test_INVARIANT_01_private.h"




#include "caos_contract.h"
void Range_init(Range* pthis, int low, int high)

{
    pthis->low = low;
    pthis->high = high;
 CAOS_ENSURE(pthis->low <= pthis->high); CAOS_ENSURE(pthis->low >= 0); }

void Range_widen(Range* pthis, int d)
{ CAOS_REQUIRE(pthis->low <= pthis->high); CAOS_REQUIRE(pthis->low >= 0);
    pthis->low -= d;
    pthis->high += d;
 CAOS_ENSURE(pthis->low <= pthis->high); CAOS_ENSURE(pthis->low >= 0); }

int Range_width(Range* pthis)

{ int result; CAOS_REQUIRE(pthis->low <= pthis->high); CAOS_REQUIRE(pthis->low >= 0);
    { result = pthis->high - pthis->low; CAOS_ENSURE(result >= 0); CAOS_ENSURE(pthis->low <= pthis->high); CAOS_ENSURE(pthis->low >= 0); return result; }
}

int Range_peekLow(const Range* pthis)
{ CAOS_REQUIRE(pthis->low <= pthis->high); CAOS_REQUIRE(pthis->low >= 0);
    return pthis->low;
}

void Range_scribble(Range* pthis)
{
    pthis->low = pthis->high + 1;
}

void Range_breakIt(Range* pthis)
{ CAOS_REQUIRE(pthis->low <= pthis->high); CAOS_REQUIRE(pthis->low >= 0);
    Range_scribble(pthis);
 CAOS_ENSURE(pthis->low <= pthis->high); CAOS_ENSURE(pthis->low >= 0); }

int Range_make(int v)
{
    return v;
}


int main(void)
{
    Range r;
    Range_init(&r, 2, 5);                    /* no entry check : the object was not valid yet */
    assert(entryFailed == 0 && exitFailed == 0);
    assert(Range_width(&r) == 3);
    assert(entryFailed == 0 && exitFailed == 0);
    Range_widen(&r, 1);                      /* low becomes 1, high 6 : still valid */
    assert(entryFailed == 0 && exitFailed == 0);
    Range_widen(&r, 5);                      /* low becomes -4 : the exit check fails (low >= 0) */
    assert(entryFailed == 0 && exitFailed == 1);
    Range_peekLow(&r);                       /* a const method : checked on entry only */
    assert(entryFailed == 1 && exitFailed == 1);
    r.low = 100;                             /* corrupted from outside */
    Range_width(&r);
    assert(entryFailed == 2);
    Range_breakIt(&r);                       /* the private method is not checked */
    assert(Range_make(7) == 7);              /* a class method is not checked */
    return 0;
}
