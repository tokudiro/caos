#include "SBuf.h"
/**
* @defgroup SBuf
* @{
*/

//Class:SBuf
typedef struct SBuf_struct{
	char str[MAX_BUF];
	boolean exist;
	int len;
}SBuf_struct_;

/** @} */
