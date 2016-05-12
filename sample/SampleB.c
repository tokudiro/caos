#include "SampleB_define.h"
#include "SampleB_private.h"
/* class SampleB */
static struct SampleB_struct instance;
SampleB* SampleB_getInstance() { return &instance; }
#line 1 "SampleB.caos"


#line 3 "SampleB.caos"
int SampleB_function(SampleB* this){
	return 9;
}


