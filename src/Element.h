#ifndef ELEMENT_H__
#define ELEMENT_H__
#include "common.h"
typedef struct Element Element;

Element* Element_new();

void Element_delete(Element* pthis);

void Element_init(Element* pthis, const char* string, int length, int type);

char* Element_string(Element* pthis);

void Element_finish(Element* pthis);
struct Element{
    char* string;
    int length;
    int type;
};
#endif /* ELEMENT_H__ */
