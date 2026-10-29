#ifndef TEST_CALL_01_H__
#define TEST_CALL_01_H__
typedef struct Counter_struct Counter;
void Counter_add(Counter* pthis, int d);
int Counter_get(Counter* pthis);
int Counter_label(Counter* pthis, const char* s, char c);
typedef struct Holder_struct Holder;
void Holder_setup(Holder* pthis, Counter* c);
int Holder_run(Holder* pthis, Counter* other, int times);
struct Source_vtable {
    int (*next)(void* self, int step);
};
typedef struct { const struct Source_vtable* vt; void* self; } Source;
int Source_next(Source r, int step);
typedef struct Seq_struct Seq;
int Seq_next(Seq* pthis, int step);
Source Seq_as_Source(Seq* p);
typedef struct Puller_struct Puller;
int Puller_pull(Puller* pthis, Source s);
int Puller_pullPtr(Puller* pthis, Source* s);
#endif /* TEST_CALL_01_H__ */
