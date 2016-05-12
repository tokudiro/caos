#include <stdio.h>

typedef struct A_struct {
	int x;
}A;

#include "Bad_define.h"
#include "Bad_private.h"
/* class Bad */
static struct Bad_struct instance;
Bad* Bad_getInstance() { return &instance; }


int Bad_get_z(const Bad* this) { return this->z;}
A* Bad_get_a(const Bad* this) { return this->a;}

