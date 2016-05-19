//Class:RBuf
typedef struct RBuf_struct RBuf;
void RBuf_init(RBuf* this, boolean isThisPointer, boolean isVerbose);
void RBuf_enque(RBuf* this, const char* str, const int len, const buftype type);
char* RBuf_deque(RBuf* this, buftype* p_type) ;
boolean RBuf_empty(const RBuf* this);
char* RBuf_allque(RBuf* this, char* buf, const char* class_buf_str) ;
boolean RBuf_back_retype(RBuf* this, const buftype find, const buftype replace) ;
const char* RBuf_back_getStr(const RBuf* this, const buftype find) ;
void RBuf_trimque(RBuf* this);

char* RBuf_toString(const RBuf* this, char* buf);
