#include <string.h>

#include "common.h"

#include "SBuf_define.h"

void SBuf_init(SBuf self) {
	memset(&(self->str[0]), 0, MAX_BUF);
	self->exist = FALSE;
	self->len = 0;
}

void SBuf_setBuf(SBuf self, char* top, int len) {
	strncpy(self->str, top, len);
	self->len = len;
	self->exist = TRUE;
}

char* SBuf_getStr(SBuf self) {
	return self->str;
}

int SBuf_getLen(SBuf self) {
	return self->len;
}
