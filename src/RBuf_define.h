#include "RBuf.h"

/**
* @defgroup RBuf
* @{
*/

//Class:RBuf
struct RBuf_struct{
	char buf[MAX_TMP][MAX_BUF];   /* buf */
	buftype type[MAX_TMP];
	boolean isThisPointer;
    boolean isVerbose;
	int first;
	int last;
};

/** @} */
