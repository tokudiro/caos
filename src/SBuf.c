#include <string.h>

#include "common.h"

/**
* @class SBuf
* @brief String Buffer
*/
#include "SBuf.h"
#include "SBuf_define.h"



/**
* @brief initial
* @memeberof SBuf
*/
void SBuf_init(SBuf* pthis) {
	memset(&(pthis->str[0]), 0, MAX_TEXT);
	pthis->exist = FALSE;
	pthis->len = 0;
}

/**
* @brief set String
* @memeberof SBuf
*/
void SBuf_setBuf(SBuf* pthis, const char* top, const int len) {
	strncpy(pthis->str, top, len);
	pthis->len = len;
	pthis->exist = TRUE;
}

/**
* @brief get String
* @memeberof SBuf
*/
const char* SBuf_getStr(const SBuf* pthis) {
	return pthis->str;
}

/**
* @brief get Length
* @memeberof SBuf
*/
int SBuf_getLen(const SBuf* pthis) {
	return pthis->len;
}


