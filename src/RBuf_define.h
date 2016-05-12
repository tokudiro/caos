#include "RBuf.h"

/**
* @defgroup RBuf
* @{
*/

//Class:RBuf
struct RBuf_struct{
	char buf[MAX_TMP][MAX_BUF];   /* buf */
	char type[MAX_TMP];
	boolean isThisPointer;
	int first;
	int last;
};

/** @} */
