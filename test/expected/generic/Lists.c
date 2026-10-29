#include <stdlib.h>

#include "Lists.h"
#include "Lists_define.h"
#include "Lists_private.h"


void IntList_init(IntList* pthis)
{
    pthis->length = 0;
    pthis->items = 0;
}

int IntList_append(IntList* pthis, int item)
{
    int* p = (int*)realloc(pthis->items, sizeof(int) * (pthis->length + 1));
    if (p == 0) {
        return -1;
    }
    pthis->items = p;
    pthis->items[pthis->length++] = item;
    return 0;
}

int IntList_at(const IntList* pthis, int index)
{
    return pthis->items[index];
}

int IntList_length(IntList* pthis)
{
    return pthis->length;
}

void IntList_finish(IntList* pthis)
{
    free(pthis->items);
}



void StrList_init(StrList* pthis)
{
    pthis->length = 0;
    pthis->items = 0;
}

int StrList_append(StrList* pthis, const char* item)
{
    const char** p = (const char**)realloc(pthis->items, sizeof(const char*) * (pthis->length + 1));
    if (p == 0) {
        return -1;
    }
    pthis->items = p;
    pthis->items[pthis->length++] = item;
    return 0;
}

const char* StrList_at(const StrList* pthis, int index)
{
    return pthis->items[index];
}

int StrList_length(StrList* pthis)
{
    return pthis->length;
}

void StrList_finish(StrList* pthis)
{
    free(pthis->items);
}

