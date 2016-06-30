#ifndef SBUF_H__
#define SBUF_H__
typedef struct SBuf_struct SBuf;
void SBuf_init(SBuf* this) ;
void SBuf_setBuf(SBuf* this, const char* top, const int len) ;
const char* SBuf_getStr(const SBuf* this) ;
int SBuf_getLen(const SBuf* this) ;
#endif /* SBUF_H__ */
