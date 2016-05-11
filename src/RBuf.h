//Class:RBuf
typedef struct RBuf_struct* RBuf;
void RBuf_init(RBuf self, boolean isThisPointer);
void RBuf_enque(RBuf self, char* str, int len, buftype type);
char* RBuf_deque(RBuf self, buftype* p_type) ;
boolean RBuf_empty(RBuf self);
char* RBuf_allque(RBuf self, char* buf, char* class_buf_str) ;
boolean RBuf_back_retype(RBuf self, buftype find, buftype replace) ;
char* RBuf_back_getStr(RBuf self, buftype find) ;

char* RBuf_toString(RBuf self, char* buf);
