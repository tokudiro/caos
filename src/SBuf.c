#include <string.h>

#include "common.h"

/**
* @class SBuf
* @brief String Buffer
*/
#include "SBuf_define.h"
#include "SBuf.h"

/**
* @brief initial
* @memeberof SBuf
*/
void SBuf_init(SBuf* this) {
	memset(&(this->str[0]), 0, MAX_TEXT);
	this->exist = FALSE;
	this->len = 0;
}

/**
* @brief set String
* @memeberof SBuf
*/
void SBuf_setBuf(SBuf* this, const char* top, const int len) {
	strncpy(this->str, top, len);
	this->len = len;
	this->exist = TRUE;
}

/**
* @brief get String
* @memeberof SBuf
*/
const char* SBuf_getStr(const SBuf* this) {
	return this->str;
}

/**
* @brief get Length
* @memeberof SBuf
*/
int SBuf_getLen(const SBuf* this) {
	return this->len;
}

