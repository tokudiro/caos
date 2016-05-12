#include <stdio.h>

typedef struct A_struct A;

#include "Bad.h"

int main(void) {
	Bad* obj = Bad_getInstance();
	//obj->z = 5;
	printf("%d", Bad_get_z(obj) );
}
