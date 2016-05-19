#include <string.h>

#include "common.h"

#include "SBuf_define.h"

/**
* @addtogroup SBuf
* @brief String Buffer
* @{
*/

void SBuf_init(SBuf* this) {
	memset(&(this->str[0]), 0, MAX_TEXT);
	this->exist = FALSE;
	this->len = 0;
}

void SBuf_setBuf(SBuf* this, const char* top, const int len) {
	strncpy(this->str, top, len);
	this->len = len;
	this->exist = TRUE;
}

const char* SBuf_getStr(const SBuf* this) {
	return this->str;
}

int SBuf_getLen(const SBuf* this) {
	return this->len;
}

/** @} */
