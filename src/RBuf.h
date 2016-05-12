//Class:RBuf
typedef struct RBuf_struct RBuf;
void RBuf_init(RBuf* this, boolean isThisPointer);
void RBuf_enque(RBuf* this, const char* str, const int len, const buftype type);
char* RBuf_deque(RBuf* this, buftype* p_type) ;
boolean RBuf_empty(RBuf* this);
char* RBuf_allque(RBuf* this, char* buf, char* class_buf_str) ;
boolean RBuf_back_retype(RBuf* this, buftype find, buftype replace) ;
char* RBuf_back_getStr(RBuf* this, buftype find) ;

char* RBuf_toString(RBuf* this, char* buf);
