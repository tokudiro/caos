/*
 * Caos class : holds the state shared while a source is translated.
 *
 * Caos.c, Caos.h and Caos_define.h are generated from this file by caos
 * itself (make regen). The generated files are committed so that caos can
 * be built without an existing caos.
 */
#include "ListBuf.h"
#include "Caos.h"
#include "Caos_define.h"
static struct Caos_struct instance;
Caos* Caos_getInstance() { return &instance; }



void Caos_init(Caos* pthis){
    pthis->list = ListBuf_getInstance();
}

ListBuf* Caos_list(Caos* pthis){
    return pthis->list;
}


