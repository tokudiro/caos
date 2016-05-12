#include <stdio.h>
#include  "SampleB.h"


#include "SampleA_define.h"
#include "SampleA_private.h"
/* class SampleA */
#line 4 "SampleA.caos"



#line 8 "SampleA.caos"
void SampleA_function(SampleA* this, int attr){
	this->b = SampleB_getInstance();
	printf( "%d\n", SampleB_function(this->b) );
}


