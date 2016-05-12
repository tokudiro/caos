#include  "B.h"

/* クラスA */
#include "A_def.h"
//Class:A


int A_func(A* this, C* c) {
  B* b;
  B_func(b, c);
  return 0;
}

