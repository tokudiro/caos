#include <string.h>

#include "boolean.h"
#include "buftype.h"
#include "common.h"

#include "TestBuf_define.h"
//Class:TestBuf

/**
 * initial
 */
void TestBuf_init(TestBuf* this){
	this->first = 0;
	this->last = 0;
}

void TestBuf_enque(TestBuf* this, char* str, int len, buftype type){
	memset(this->buf[this->last], 0, MAX_BUF);
	strncpy( &(this->buf[this->last][0]), str, len);
	this->type[this->last] = type;
	this->last++;
	if (this->last==MAX_TMP) {this->last=0;}
}

char* TestBuf_deque(TestBuf* this, buftype* p_type){
	char* ret = this->buf[this->first];
	*p_type = this->type[this->first];
	this->first++;
	if (this->first==MAX_TMP) {this->first=0;}
	return ret;
}

boolean TestBuf_empty(TestBuf* this) {
	return (this->first==this->last?TRUE:FALSE);
}

char* TestBuf_allque(TestBuf* this, char* buf){
	buftype type;
	char* str;
	while(TestBuf_empty(this) == FALSE) {
		str = TestBuf_deque(this, &type);
		if (type == OBJECT) {
			printf("obj_%s",  str);
		} else {
			printf("%s",  str);
		}
	}
}
