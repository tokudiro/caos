#include <stdio.h>
#include <assert.h>

#include "test_COMMENT_01.h"
#include "test_COMMENT_01_define.h"
#include "test_COMMENT_01_private.h"


/* standalone block comment */
int TEST_COMMENT_01_get_e(const TEST_COMMENT_01* pthis) { return pthis->e; }
void TEST_COMMENT_01_set_e(TEST_COMMENT_01* pthis, int e) { pthis->e = e; }
int TEST_COMMENT_01_get_f(const TEST_COMMENT_01* pthis) { return pthis->f; }
void TEST_COMMENT_01_set_f(TEST_COMMENT_01* pthis, int f) { pthis->f = f; }
int TEST_COMMENT_01_sum(TEST_COMMENT_01* pthis){
    return pthis->a + pthis->b + pthis->c;
}



int main(){
    TEST_COMMENT_01 obj;
    TEST_COMMENT_01_set_e(&obj, 3);
    assert(TEST_COMMENT_01_get_e(&obj) == 3);
    return 0;
}
