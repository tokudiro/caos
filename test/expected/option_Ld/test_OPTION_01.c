#include <stdio.h>
#include <assert.h>

#include "test_OPTION_01.h"
#include "test_OPTION_01_private.h"


// line comment before a declaration
int TEST_OPTION_01_get_c(const TEST_OPTION_01* pthis) { return pthis->c; }
void TEST_OPTION_01_set_c(TEST_OPTION_01* pthis, int c) { pthis->c = c; }
int TEST_OPTION_01_sum(TEST_OPTION_01* pthis){
    // @a in a line comment
    return pthis->a + pthis->b + pthis->c; /* block */
}

int TEST_OPTION_01_twice(TEST_OPTION_01* pthis, int x){
    return x * 2; // private body
}

int TEST_OPTION_01_calc(TEST_OPTION_01* pthis){
    return TEST_OPTION_01_twice(pthis, TEST_OPTION_01_sum(pthis));
}



int main(){
    TEST_OPTION_01 obj;
    obj.a = 1; obj.b = 2;
    TEST_OPTION_01_set_c(&obj, 3);
    assert(TEST_OPTION_01_sum(&obj) == 6);
    assert(TEST_OPTION_01_calc(&obj) == 12);
    return 0;
}
