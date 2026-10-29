#include <assert.h>
#include <string.h>
#include "Lists.h"
#include "Lists_define.h"

int main(void)
{
    IntList a;
    StrList b;
    IntList_init(&a);
    StrList_init(&b);
    assert(IntList_append(&a, 3) == 0);
    assert(IntList_append(&a, 4) == 0);
    assert(StrList_append(&b, "x") == 0);
    assert(IntList_length(&a) == 2);
    assert(IntList_at(&a, 1) == 4);
    assert(strcmp(StrList_at(&b, 0), "x") == 0);
    IntList_finish(&a);
    StrList_finish(&b);
    return 0;
}
