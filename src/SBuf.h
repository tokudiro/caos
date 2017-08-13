#ifndef SBUF_H__
#define SBUF_H__
typedef struct SBuf_struct SBuf;
void SBuf_init(SBuf* pthis) ;
void SBuf_setBuf(SBuf* pthis, const char* top, const int len) ;
const char* SBuf_getStr(const SBuf* pthis) ;
int SBuf_getLen(const SBuf* pthis) ;
#endif /* SBUF_H__ */
