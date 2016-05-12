#include <string.h>

#include "common.h"

/**
* @addtogroup RBuf
* @brief Ring Buffer
* @{
*/

#include "RBuf_define.h"
//Class:RBuf 

void RBuf_init(RBuf* this, const boolean isThisPointer){
	this->first = 0;
	this->last = 0;
	this->isThisPointer = isThisPointer;
}

void RBuf_enque(RBuf* this, const char* str, const int len, const buftype type){
	memset(&(this->buf[this->last][0]), 0, MAX_BUF);
	strcpy( &(this->buf[this->last][0]), str);
	this->type[this->last] = type;
	this->last++;
	if (this->first == this->last) { /* buffer over */ }
	if (this->last==MAX_TMP) {this->last=0;}
}

char* RBuf_deque(RBuf* this, buftype* p_type) {
	char* ret = this->buf[this->first];
	*p_type = this->type[this->first];
	this->first++;
	if (this->first==MAX_TMP) {this->first=0;}
	return ret;
}

boolean RBuf_empty(const RBuf* this){
	return (this->first==this->last?TRUE:FALSE);
}

char* RBuf_allque(RBuf* this, char* buf, const char* class_buf_str) {
	buftype type;
	char* str;
	buf[0] = 0;
	while(RBuf_empty(this) == FALSE) {
		str = RBuf_deque(this, &type);
		switch(type) {
		case T_OBJECT:
			if (this->isThisPointer) {
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

boolean RBuf_back_retype(RBuf* this, const buftype find, const buftype replace) {
	if (RBuf_empty(this) ) return FALSE;
	
	int index = this->last;
	while( index != this->first) {
		index--;
		if ( index < 0 ) index=MAX_TMP;
		if ( this->type[index] == T_DOT ) { return FALSE; }
		if ( this->type[index] == find ) {
			this->type[index] = replace;
			return TRUE;
		}
	}
	return FALSE;
}

const char* RBuf_back_getStr(const RBuf* this, const buftype find) {
	if (RBuf_empty(this) ) return 0;
	
	int index = this->last;
	while( index != this->first) {
		index--;
		if ( index < 0 ) index=MAX_TMP;
		if ( this->type[index] == find ) {
			return this->buf[index];
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

/**
* @brief toString
* @param buf A string that represents the current object.
* @return A string that represents the current object.
*/
char* RBuf_toString(RBuf* this, char* buf) {
	if (this->first == this->last) return buf;

	if (this->first < this->last) {
		for (int i = this->first; i<=this->last; i++)
		{
			strcat(buf, TYPE_CAPTION[this->type[i]]);
			strcat(buf, ":");
			strcat(buf, this->buf[i]);
			strcat(buf, "\n");
		}
	} else {
		for (int i = this->first; i<MAX_BUF; i++)
		{
			strcat(buf, TYPE_CAPTION[this->type[i]]);
			strcat(buf, ":");
			strcat(buf, this->buf[i]);
			strcat(buf, "\n");
		}
		for (int i = 0; i<=this->last; i++)
		{
			strcat(buf, TYPE_CAPTION[this->type[i]]);
			strcat(buf, ":");
			strcat(buf, this->buf[i]);
			strcat(buf, "\n");
		}
	}
	return buf;
}

/** @} */
