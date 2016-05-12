#include <string.h>

#include "common.h"

#include "RBuf_define.h"
//Class:RBuf 

/**
* @defgroup RBuf
* @{
*/

/**
* @brief initial
*/
void RBuf_init(RBuf self, boolean isThisPointer){
	self->first = 0;
	self->last = 0;
	self->isThisPointer = isThisPointer;
}

void RBuf_enque(RBuf self, char* str, int len, buftype type){
	memset(&(self->buf[self->last][0]), 0, MAX_BUF);
	strcpy( &(self->buf[self->last][0]), str);
	self->type[self->last] = type;
	self->last++;
	if (self->first == self->last) { /* buffer over */ }
	if (self->last==MAX_TMP) {self->last=0;}
}

char* RBuf_deque(RBuf self, buftype* p_type) {
	char* ret = self->buf[self->first];
	*p_type = self->type[self->first];
	self->first++;
	if (self->first==MAX_TMP) {self->first=0;}
	return ret;
}

boolean RBuf_empty(RBuf self){
	return (self->first==self->last?TRUE:FALSE);
}

char* RBuf_allque(RBuf self, char* buf, char* class_buf_str) {
	buftype type;
	char* str;
	buf[0] = 0;
	while(RBuf_empty(self) == FALSE) {
		str = RBuf_deque(self, &type);
		switch(type) {
		case T_OBJECT:
			if (self->isThisPointer) {
				strcat(buf, "this->");
			}else{
				strcat(buf, "self->");
			}
			break;
		case T_METHOD:
			strcat(buf, class_buf_str);
			strcat(buf, "_");
			break;
		}
		strcat(buf,  str);
	}
	return buf;
}

boolean RBuf_back_retype(RBuf self, buftype find, buftype replace) {
	if (RBuf_empty(self) ) return FALSE;
	
	int index = self->last;
	while( index != self->first) {
		index--;
		if ( index < 0 ) index=MAX_TMP;
		if ( self->type[index] == T_DOT ) { return FALSE; }
		if ( self->type[index] == find ) {
			self->type[index] = replace;
			return TRUE;
		}
	}
	return FALSE;
}

char* RBuf_back_getStr(RBuf self, buftype find) {
	if (RBuf_empty(self) ) return 0;
	
	int index = self->last;
	while( index != self->first) {
		index--;
		if ( index < 0 ) index=MAX_TMP;
		if ( self->type[index] == find ) {
			return self->buf[index];
		}
	}
	return 0;
}

static char* TYPE_CAPTION[] = {
	"T_NON",
	"T_NUMBER",
	"T_WORD",
	"T_OBJECT",
	"T_CLASS", 
	"T_DOT",
	"T_KEYWORD",
	"T_METHOD",
	"T_PARAMETER",
	"T_ASTERISK",
	"T_TYPE"};

char* RBuf_toString(RBuf self, char* buf) {
	if (self->first == self->last) return buf;

	if (self->first < self->last) {
		for (int i = self->first; i<=self->last; i++)
		{
			strcat(buf, TYPE_CAPTION[self->type[i]]);
			strcat(buf, ":");
			strcat(buf, self->buf[i]);
			strcat(buf, "\n");
		}
	} else {
		for (int i = self->first; i<MAX_BUF; i++)
		{
			strcat(buf, TYPE_CAPTION[self->type[i]]);
			strcat(buf, ":");
			strcat(buf, self->buf[i]);
			strcat(buf, "\n");
		}
		for (int i = 0; i<=self->last; i++)
		{
			strcat(buf, TYPE_CAPTION[self->type[i]]);
			strcat(buf, ":");
			strcat(buf, self->buf[i]);
			strcat(buf, "\n");
		}
	}
	return buf;
}

/** @} */
