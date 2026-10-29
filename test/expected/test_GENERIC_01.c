#include <assert.h>

// a pair of types, a struct, and a method that uses $Name and a type argument


#include "test_GENERIC_01.h"
#include "test_GENERIC_01_private.h"

void Counter_inc(Counter* pthis){ pthis->n++; }




int IntChar_getFirst(IntChar* pthis)
{
    return pthis->first;
}

void IntChar_set(IntChar* pthis, int a, char b)
{
    pthis->first = a;
    pthis->second = b;
}



Counter* CounterPtr_getFirst(CounterPtr* pthis)
{
    return pthis->first;
}

void CounterPtr_set(CounterPtr* pthis, Counter* a, int b)
{
    pthis->first = a;
    pthis->second = b;
}


int main(void)
{
    IntChar p;
    CounterPtr q;
    Counter c = {0};
    IntChar_set(&p, 7, 'x');
    assert(IntChar_getFirst(&p) == 7);
    CounterPtr_set(&q, &c, 1);
    Counter_inc(CounterPtr_getFirst(&q));
    assert(c.n == 1);
    return 0;
}
