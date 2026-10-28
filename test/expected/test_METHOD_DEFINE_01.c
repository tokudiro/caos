#include "test_METHOD_DEFINE_01.h"
#include "test_METHOD_DEFINE_01_define.h"
#include "test_METHOD_DEFINE_01_private.h"


void TEST_METHOD_DEFINE_01_publicMethod(TEST_METHOD_DEFINE_01* pthis){
}

void TEST_METHOD_DEFINE_01_publicConstMethod(const TEST_METHOD_DEFINE_01* pthis){
}

void TEST_METHOD_DEFINE_01_privateMethod(TEST_METHOD_DEFINE_01* pthis){
}

void TEST_METHOD_DEFINE_01_classMethod(){
}



int main(){
    TEST_METHOD_DEFINE_01_publicMethod(0);
    TEST_METHOD_DEFINE_01_publicConstMethod(0);
    TEST_METHOD_DEFINE_01_privateMethod(0);
    TEST_METHOD_DEFINE_01_classMethod();
}
