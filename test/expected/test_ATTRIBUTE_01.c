#include <assert.h>
#include <stdint.h>
#include <string.h>

#include "test_ATTRIBUTE_01.h"
#include "test_ATTRIBUTE_01_define.h"
#include "test_ATTRIBUTE_01_private.h"


int TEST_ATTRIBUTE_01_get_i(const TEST_ATTRIBUTE_01* pthis) { return pthis->i; }
void TEST_ATTRIBUTE_01_set_i(TEST_ATTRIBUTE_01* pthis, int i) { pthis->i = i; }
unsigned int TEST_ATTRIBUTE_01_get_u(const TEST_ATTRIBUTE_01* pthis) { return pthis->u; }
void TEST_ATTRIBUTE_01_set_u(TEST_ATTRIBUTE_01* pthis, unsigned int u) { pthis->u = u; }
uint32_t TEST_ATTRIBUTE_01_get_w(const TEST_ATTRIBUTE_01* pthis) { return pthis->w; }
void TEST_ATTRIBUTE_01_set_w(TEST_ATTRIBUTE_01* pthis, uint32_t w) { pthis->w = w; }
char* TEST_ATTRIBUTE_01_get_p(const TEST_ATTRIBUTE_01* pthis) { return pthis->p; }
void TEST_ATTRIBUTE_01_set_p(TEST_ATTRIBUTE_01* pthis, char* p) { pthis->p = p; }
const char* TEST_ATTRIBUTE_01_get_name(const TEST_ATTRIBUTE_01* pthis) { return pthis->name; }
void TEST_ATTRIBUTE_01_set_name(TEST_ATTRIBUTE_01* pthis, const char* name) { pthis->name = name; }
struct TEST_ATTRIBUTE_01_struct* TEST_ATTRIBUTE_01_get_self(const TEST_ATTRIBUTE_01* pthis) { return pthis->self; }
void TEST_ATTRIBUTE_01_set_self(TEST_ATTRIBUTE_01* pthis, struct TEST_ATTRIBUTE_01_struct* self) { pthis->self = self; }
const int TEST_ATTRIBUTE_01_get_fixed(const TEST_ATTRIBUTE_01* pthis) { return pthis->fixed; }

int TEST_ATTRIBUTE_01_sum(TEST_ATTRIBUTE_01* pthis){
    return pthis->i + (int)pthis->u + (int)pthis->w;
}



int main(){
    TEST_ATTRIBUTE_01 obj;
    memset(&obj, 0, sizeof(obj));
    TEST_ATTRIBUTE_01_set_i(&obj, 1);
    TEST_ATTRIBUTE_01_set_u(&obj, 2u);
    TEST_ATTRIBUTE_01_set_w(&obj, 3);
    TEST_ATTRIBUTE_01_set_p(&obj, 0);
    TEST_ATTRIBUTE_01_set_name(&obj, "abc");
    TEST_ATTRIBUTE_01_set_self(&obj, &obj);
    assert(TEST_ATTRIBUTE_01_get_i(&obj) == 1);
    assert(TEST_ATTRIBUTE_01_get_u(&obj) == 2u);
    assert(TEST_ATTRIBUTE_01_get_w(&obj) == 3);
    assert(TEST_ATTRIBUTE_01_get_p(&obj) == 0);
    assert(strcmp(TEST_ATTRIBUTE_01_get_name(&obj), "abc") == 0);
    assert(TEST_ATTRIBUTE_01_get_self(&obj) == &obj);
    assert(TEST_ATTRIBUTE_01_get_fixed(&obj) == 0);
    assert(TEST_ATTRIBUTE_01_sum(&obj) == 6);
    return 0;
}
