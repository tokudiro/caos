#include "File.h"
#include "File_define.h"
#include "File_private.h"

int File_read(File* pthis, char* buf, int len){ (void)buf; pthis->pos += len; return len; }
void File_reset(File* pthis){ pthis->pos = 0; }
static int File_Reader_read(void* self, char* buf, int len)
{
    return File_read((File*)self, buf, len);
}
static void File_Reader_reset(void* self)
{
    File_reset((File*)self);
}
static const struct Reader_vtable File_Reader_vt = { File_Reader_read, File_Reader_reset };
Reader File_as_Reader(File* p)
{
    Reader r;
    r.vt = &File_Reader_vt;
    r.self = p;
    return r;
}

