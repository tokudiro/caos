#include "Reader.h"
int Reader_read(Reader r, char* buf, int len)
{
    return r.vt->read(r.self, buf, len);
}
void Reader_reset(Reader r)
{
    r.vt->reset(r.self);
}

