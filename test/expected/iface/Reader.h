#ifndef READER_H__
#define READER_H__
struct Reader_vtable {
    int (*read)(void* self, char* buf, int len);
    void (*reset)(void* self);
};
typedef struct { const struct Reader_vtable* vt; void* self; } Reader;
int Reader_read(Reader r, char* buf, int len);
void Reader_reset(Reader r);
#endif /* READER_H__ */
