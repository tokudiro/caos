#include "RBuf.h"

/**
* @defgroup RBuf
* @{
*/

//Class:RBuf
struct RBuf_struct{
	char buf[MAX_ARRAYSIZE][MAX_TEXT];   /* buf */
	buftype type[MAX_ARRAYSIZE];
	boolean isThisPointer;
    boolean isVerbose;
	int first;
	int last;
};

/** @} */
