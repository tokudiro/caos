#ifndef MOCK_H__
#define MOCK_H__
typedef struct Mock_struct Mock;
#include "Reader.h"
int Mock_read(Mock* pthis, char* buf, int len);
void Mock_reset(const Mock* pthis);
Reader Mock_as_Reader(Mock* p);
#endif /* MOCK_H__ */
