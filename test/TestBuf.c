#include <stdio.h>
#include <string.h>

#include "boolean.h"
#include "buftype.h"
#include "common.h"

#include "TestBuf_define.h"
#include "TestBuf_private.h"
/* class TestBuffer */
static struct TestBuffer_struct instance;
TestBuffer TestBuffer_getInstance() { return &instance; }
#line 8 "TestBuf.caos"


/**
 * initial
 */
#line 17 "TestBuf.caos"
void TestBuffer_init(TestBuffer self){
	self->first = 0;
	self->last = 0;
}

// enque
#line 23 "TestBuf.caos"
void TestBuffer_enque(TestBuffer self, char* str, int len, buftype type){
	memset(&(self->buf[self->last][0]), 0, MAX_BUF);
	strcpy( &(self->buf[self->last][0]), str);
	self->type[self->last] = type;
	self->last++;
	if (self->first == self->last) {/* buffer over */;}
	if (self->last==MAX_TMP) {self->last=0;}
}

#line 32 "TestBuf.caos"
char* TestBuffer_deque(TestBuffer self, buftype* p_type) {
	char* ret = self->buf[self->first];
	*p_type = self->type[self->first];
	self->first++;
	if (self->first==MAX_TMP) {self->first=0;}
	return ret;
}

#line 40 "TestBuf.caos"
boolean TestBuffer_empty(TestBuffer self){
	return (self->first==self->last?TRUE:FALSE);
}

#line 44 "TestBuf.caos"
char* TestBuffer_allque(TestBuffer self, char* buf, char* class_buf_str) {
	buftype type;
	char* str;
	buf[0] = 0;
	while(TestBuffer_empty(self) == FALSE) {
		str = TestBuffer_deque(self, &type);
		switch(type) {
		case T_OBJECT:
			strcat(buf, "this->");
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

#line 64 "TestBuf.caos"
boolean TestBuffer_back_retype(TestBuffer self, buftype find, buftype replace) {
	if (TestBuffer_empty(self) ) return FALSE;
	
	// ƒRƒƒ“ƒg
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

#line 81 "TestBuf.caos"
int TestBuffer_public(TestBuffer self) {
	return TestBuffer_private(self);
}

#line 85 "TestBuf.caos"
int TestBuffer_private(TestBuffer self) {
	return 1;
}

int other_function(int x)
{
	return 0;
}



int main(void)
{
	struct TestBuffer_struct buf;
	char longbuf[100] = {0};
	TestBuffer_init(&buf);
	TestBuffer_enque(&buf, "aaa", 3, T_WORD);
	TestBuffer_enque(&buf, "bb", 2, T_WORD);
	printf("%s\n", TestBuffer_allque(&buf, longbuf, "zzz") );
	return 0;
}
