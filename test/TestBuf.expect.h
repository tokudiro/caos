#ifndef __TestBuf__
#define __TestBuf__
//Class:TestBuf
typedef struct TestBuf_Struct TestBuf;
void TestBuf_init(TestBuf this);
void TestBuf_enque(TestBuf this, char* str, int len, buftype type);
char* TestBuf_deque(TestBuf this, buftype* p_type) ;
boolean TestBuf_empty(TestBuf this);
char* TestBuf_allque(TestBuf this, char* buf, char* class_buf_str);
boolean TestBuf_back_retype(TestBuf this, buftype find, buftype replace);
#endif
