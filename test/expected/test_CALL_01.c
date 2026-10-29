#include <assert.h>
#include <string.h>

#include "test_CALL_01.h"
#include "test_CALL_01_define.h"
#include "test_CALL_01_private.h"

void Counter_add(Counter* pthis, int d){ pthis->n += d; }
int Counter_get(Counter* pthis){ return pthis->n; }
int Counter_label(Counter* pthis, const char* s, char c){ return (int)strlen(s) + (c == 'x'); }



void Holder_setup(Holder* pthis, Counter* c){ pthis->inner = c; pthis->own.n = 0; }int Holder_run(Holder* pthis, Counter* other, int times)
{
    Counter local;            // a value
    Counter* p = other;       // a pointer
    int i;
    local.n = 0;
    for (i = 0; i < times; i++) {
        Counter_add(p, 1);
        Counter_add(&local, 2);
        Counter_add(pthis->inner, 3);
        Counter_add(&pthis->own, 4);
    }
    return Counter_get(&local) + Counter_get(p) + Counter_get(pthis->inner) + Counter_get(&pthis->own) + Counter_label(p, "abc", 'x') - 4;
}


// a variable that is not of a class is left alone
typedef struct { int (*fp)(int); } Ops;
static int twice(int x) { return x * 2; }

int Source_next(Source r, int step)
{
    return r.vt->next(r.self, step);
}



int Seq_next(Seq* pthis, int step){ pthis->cur += step; return pthis->cur; }
static int Seq_Source_next(void* self, int step)
{
    return Seq_next((Seq*)self, step);
}
static const struct Source_vtable Seq_Source_vt = { Seq_Source_next };
Source Seq_as_Source(Seq* p)
{
    Source r;
    r.vt = &Seq_Source_vt;
    r.self = p;
    return r;
}



int Puller_pull(Puller* pthis, Source s){ return Source_next(s, 1) + Source_next(s, 2); }
int Puller_pullPtr(Puller* pthis, Source* s){ return Source_next(*s, 5); }


int main(void)
{
    Counter a = {0};
    Counter b = {0};
    Holder h;
    Ops ops;
    Seq q = {0};
    ops.fp = twice;
    Holder_setup(&h, &a);
    // two rounds: local 4, b 2, a 6, own 8
    assert(Holder_run(&h, &b, 2) == 20);
    assert(ops.fp(3) == 6);
    Puller pl;
    Source src = Seq_as_Source(&q);
    assert(Puller_pull(&pl, src) == 1 + 3);
    assert(Puller_pullPtr(&pl, &src) == 8);
    return 0;
}
