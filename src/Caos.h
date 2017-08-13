#ifndef CAOS_H__
#define CAOS_H__
typedef struct ListBuf_struct ListBuf;
typedef struct Caos_struct Caos;
Caos* Caos_getInstance();

void Caos_init(Caos* pthis);

ListBuf* Caos_list(Caos* pthis);
#endif /* CAOS_H__ */
