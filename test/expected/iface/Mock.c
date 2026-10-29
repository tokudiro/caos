#include <string.h>
#include "Mock.h"
#include "Mock_define.h"
#include "Mock_private.h"

int Mock_read(Mock* pthis, char* buf, int len){ strncpy(buf, pthis->data, len); return (int)strlen(pthis->data); }
void Mock_reset(const Mock* pthis){ (void)pthis->data; }
static int Mock_Reader_read(void* self, char* buf, int len)
{
    return Mock_read((Mock*)self, buf, len);
}
static void Mock_Reader_reset(void* self)
{
    Mock_reset((Mock*)self);
}
static const struct Reader_vtable Mock_Reader_vt = { Mock_Reader_read, Mock_Reader_reset };
Reader Mock_as_Reader(Mock* p)
{
    Reader r;
    r.vt = &Mock_Reader_vt;
    r.self = p;
    return r;
}

