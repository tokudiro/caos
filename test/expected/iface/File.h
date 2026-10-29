#ifndef FILE_H__
#define FILE_H__
typedef struct File_struct File;
#include "Reader.h"
int File_read(File* pthis, char* buf, int len);
void File_reset(File* pthis);
Reader File_as_Reader(File* p);
#endif /* FILE_H__ */
