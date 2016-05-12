#include  "B.h"


/* クラスA */
#include "A_define.h"
#include "A_private.h"
/* class A */



int func(C* c) {
  B b;
  B_func(b, c);
  printf("@x \"");
  return 0;
}

#define X(a) %a_X()

int A_function(A* this, int x1, int x2){
	return x1+x2;
}
  #+void unfuction(void){}

#define PRINT(import) printf("#import\n")


