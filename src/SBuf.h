#ifndef __SBuf__
#define __SBuf__
/* Class:SBuf */
typedef struct SBuf_struct* SBuf;

void SBuf_init(SBuf self);
void SBuf_setBuf(SBuf self, char* top, int len);
char* SBuf_getStr(SBuf self);
int SBuf_getLen(SBuf self);

#endif /* __SBuf__ */
