#ifndef LISTBUF_H__
#define LISTBUF_H__
typedef struct Element Element;
typedef struct ListBuf_struct ListBuf;
ListBuf* ListBuf_getInstance();

void ListBuf_init(ListBuf* pthis);

void ListBuf_append(ListBuf* pthis, Element* element);

int ListBuf_length(ListBuf* pthis);

Element* ListBuf_element(ListBuf* pthis, int index);

void ListBuf_finish(ListBuf* pthis);
#endif /* LISTBUF_H__ */
