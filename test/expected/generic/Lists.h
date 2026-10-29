#ifndef LISTS_H__
#define LISTS_H__
typedef struct IntList_struct IntList;
void IntList_init(IntList* pthis);
int IntList_append(IntList* pthis, int item);
int IntList_at(const IntList* pthis, int index);
int IntList_length(IntList* pthis);
void IntList_finish(IntList* pthis);
typedef struct StrList_struct StrList;
void StrList_init(StrList* pthis);
int StrList_append(StrList* pthis, const char* item);
const char* StrList_at(const StrList* pthis, int index);
int StrList_length(StrList* pthis);
void StrList_finish(StrList* pthis);
#endif /* LISTS_H__ */
