#include <assert.h>

// T_name : the method "name" of the type argument (Counter_inc, Counter_get)
#include "test_GENERIC_02.h"
#include "test_GENERIC_02_define.h"
#include "test_GENERIC_02_private.h"

void Counter_inc(Counter* pthis){ pthis->n++; }
int Counter_get(Counter* pthis){ return pthis->n; }





int TwiceCounter_run(TwiceCounter* pthis, Counter* x)
{
    Counter_inc(x);
    Counter_inc(x);
    return Counter_get(x);
}


int main(void)
{
    TwiceCounter t;
    Counter c = {0};
    assert(TwiceCounter_run(&t, &c) == 2);
    return 0;
}
