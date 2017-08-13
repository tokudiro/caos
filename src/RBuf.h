#ifndef RBUF_H__
#define RBUF_H__
typedef struct RBuf_struct RBuf;
void RBuf_init(RBuf* pthis, const char* thispointer_str, boolean isVerbose);
void RBuf_setClass(RBuf* pthis, const char* thisclass_str);
void RBuf_enque(RBuf* pthis, const char* str, const int len, const buftype type);
char* RBuf_deque(RBuf* pthis, buftype* p_type) ;
boolean RBuf_isEmpty(const RBuf* pthis);
char* RBuf_allDeque(RBuf* pthis, char* buf) ;
boolean RBuf_back_retype(RBuf* pthis, const buftype find, const buftype replace) ;
const char* RBuf_back_getStr(const RBuf* pthis, const buftype find) ;
void RBuf_trimQue(RBuf* pthis);
boolean RBuf_isWSPACE(RBuf* pthis);
char* RBuf_toString(const RBuf* pthis, char* tostring) ;
#endif /* RBUF_H__ */
