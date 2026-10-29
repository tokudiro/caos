#include <stdio.h>
#include <assert.h>

#include "test_OPTION_01.h"
#include "test_OPTION_01_define.h"
#include "test_OPTION_01_private.h"
#line 4 "test_OPTION_01.caos"


/* line comment before a declaration*/
int TEST_OPTION_01_get_c(const TEST_OPTION_01* self) { return self->c; }
void TEST_OPTION_01_set_c(TEST_OPTION_01* self, int c) { self->c = c; }
#line 11 "test_OPTION_01.caos"
int TEST_OPTION_01_sum(TEST_OPTION_01* self){
    /* @a in a line comment*/
    return self->a + self->b + self->c; /* block */
}

#line 16 "test_OPTION_01.caos"
int TEST_OPTION_01_twice(TEST_OPTION_01* self, int x){
    return x * 2; /* private body*/
}

#line 20 "test_OPTION_01.caos"
int TEST_OPTION_01_calc(TEST_OPTION_01* self){
    return TEST_OPTION_01_twice(self, TEST_OPTION_01_sum(self));
}



int main(){
    TEST_OPTION_01 obj;
    obj.a = 1; obj.b = 2;
    TEST_OPTION_01_set_c(&obj, 3);
    assert(TEST_OPTION_01_sum(&obj) == 6);
    assert(TEST_OPTION_01_calc(&obj) == 12);
    return 0;
}
