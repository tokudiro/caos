#ifndef __SBuf__
#define __SBuf__
/* Class:SBuf */
typedef struct SBuf_struct SBuf;

void SBuf_init(SBuf* this);
void SBuf_setBuf(SBuf* this, const char* top, int len);
const char* SBuf_getStr(const SBuf* this);
int SBuf_getLen(const SBuf* this);

#endif /* __SBuf__ */
