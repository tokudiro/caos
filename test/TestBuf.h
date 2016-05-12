#ifndef __TestBuf_H__
#define __TestBuf_H__
/* class TestBuffer */
#line 8 "TestBuf.caos"
typedef struct TestBuffer_struct* TestBuffer;
TestBuffer TestBuffer_getInstance();
#line 17 "TestBuf.caos"
void TestBuffer_init(TestBuffer self);
#line 23 "TestBuf.caos"
void TestBuffer_enque(TestBuffer self, char* str, int len, buftype type);
#line 32 "TestBuf.caos"
char* TestBuffer_deque(TestBuffer self, buftype* p_type) ;
#line 40 "TestBuf.caos"
boolean TestBuffer_empty(TestBuffer self);
#line 44 "TestBuf.caos"
char* TestBuffer_allque(TestBuffer self, char* buf, char* class_buf_str) ;
#line 64 "TestBuf.caos"
boolean TestBuffer_back_retype(TestBuffer self, buftype find, buftype replace) ;
#line 81 "TestBuf.caos"
int TestBuffer_public(TestBuffer self) ;
#endif /* __TestBuf_H__ */
