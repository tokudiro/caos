#include "test_METHOD_DEFINE_02.h"
#include "test_METHOD_DEFINE_02_define.h"
#include "test_METHOD_DEFINE_02_private.h"


void TEST_METHOD_DEFINE_02_publicMethod(TEST_METHOD_DEFINE_02* pthis, int a,
    int b)
{
    
}

void TEST_METHOD_DEFINE_02_privateMethod
(TEST_METHOD_DEFINE_02* pthis, int a, int b)
{
}

void TEST_METHOD_DEFINE_02_classMethod(){
}



int main(){
    TEST_METHOD_DEFINE_02_publicMethod(0,1,2);
    TEST_METHOD_DEFINE_02_privateMethod(0,1,2);
    TEST_METHOD_DEFINE_02_classMethod();
}
