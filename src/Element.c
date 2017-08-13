#include <string.h>
#include <stdlib.h>


#include "Element.h"



Element* Element_new()
{
    return (Element*)malloc( sizeof(Element) );
}


void Element_delete(Element* pthis)
{
    if(pthis)
    {
        free(pthis);
    }
}


void Element_init(Element* pthis, const char* string, int length, int type)
{
    pthis->length = length;
    pthis->string = malloc(pthis->length+1);
    strcpy(pthis->string, string);
}


char* Element_string(Element* pthis)
{
    return pthis->string;
}


void Element_finish(Element* pthis)
{
    if(pthis->string)
    {
        free(pthis->string);
    }
}


