#include<stdio.h>

int func(int a, int b){return a+b;}

typedef struct ClassA{
    int a;
}ClassA;

#include "test_METHOD_SCOPE_01.h"
#include "test_METHOD_SCOPE_01_define.h"
#include "test_METHOD_SCOPE_01_private.h"


int TEST_METHOD_SCOPE_01_a(TEST_METHOD_SCOPE_01* pthis){return 0;}

int TEST_METHOD_SCOPE_01_func(TEST_METHOD_SCOPE_01* pthis){
	// member a
	//@a!(0); // 0x21 !   => expected ';' before '!' token
	(pthis->a%(2)); // 0x25 %
	(pthis->a&&(0)); // 0x26 &
	
	pthis->a*(2); // 0x2a *
	pthis->a+(2); // 0x2b +
	func(pthis->a,(2)); // 0x2c ,
	pthis->a-(2); // 0x2d -
	//@a.(2); // 0x2e .  => expected identifier before '(' token
	pthis->a/(2); // 0x2f /
	
	(0?pthis->a:(0)); // 0x3a :
	pthis->a;(0); // 0x3b ;
	(pthis->a<(2)); // 0x3c <
	pthis->a=(2+3); // 0x3d =
	(pthis->a>(2)); // 0x3e >
	(pthis->a?(1):0); // 0x3f ?

	(pthis->a||(0)); // 0x7c |
	//@a~(0); // 0x7e ~    => expected ';' before '~' token

	// method a
	TEST_METHOD_SCOPE_01_a(pthis);
}

int TEST_METHOD_SCOPE_01_called_func(TEST_METHOD_SCOPE_01* pthis, int z){
}

void TEST_METHOD_SCOPE_01_call_func(TEST_METHOD_SCOPE_01* pthis){
    TEST_METHOD_SCOPE_01_called_func(pthis, 0);     /* arg num */
    TEST_METHOD_SCOPE_01_called_func(pthis, -1);    /* arg - */
    TEST_METHOD_SCOPE_01_called_func(pthis, (1+2)); /* arg ( */
    TEST_METHOD_SCOPE_01_called_func(pthis, pthis->a);    /* arg object */
}

void TEST_METHOD_SCOPE_01_print_func(TEST_METHOD_SCOPE_01* pthis){
    printf("print a=%d\n", pthis->a);
    printf("\"");
}

void TEST_METHOD_SCOPE_01_func_check(TEST_METHOD_SCOPE_01* pthis){ /* func_check */
    ClassA* x = (ClassA*)0;
} /* end_func_check */



int main(){
}

// Output
// METHOD_SCOPE_TEST01.c
// METHOD_SCOPE_TEST01.h
// METHOD_SCOPE_TEST01_private.h
// METHOD_SCOPE_TEST01_define.h
