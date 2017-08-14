#include <stdlib.h>


#include "Element.h"

#include "ListBuf.h"
#include "ListBuf_define.h"
static struct ListBuf_struct instance;
ListBuf* ListBuf_getInstance() { return &instance; }




void ListBuf_init(ListBuf* pthis)
{
    pthis->length = 0;
}


void ListBuf_append(ListBuf* pthis, Element* element)
{
    pthis->length++;
    pthis->elements = realloc(pthis->elements, sizeof(Element*)*pthis->length);
    if (pthis->elements) {
        pthis->elements[pthis->length-1] = element;
    }
}


int ListBuf_length(ListBuf* pthis)
{
    return pthis->length;
}


Element* ListBuf_element(ListBuf* pthis, int index)
{
    return pthis->elements[index];
}


void ListBuf_finish(ListBuf* pthis)
{
    while ( pthis->length<0 ) {
        Element_finish(pthis->elements[pthis->length-1]);
        Element_delete(pthis->elements[pthis->length-1]);
        pthis->length--;
    }
    free(pthis->elements);
}


