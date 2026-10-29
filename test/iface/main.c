#include <assert.h>
#include <stdio.h>
#include "File.h"
#include "File_define.h"
#include "Mock.h"
#include "Mock_define.h"

/* knows only the interface */
static int total(Reader r)
{
    char b[8];
    int t = Reader_read(r, b, 4) + Reader_read(r, b, 4);
    Reader_reset(r);
    return t;
}

int main(void)
{
    File f = {0};
    Mock m = {"ab"};
    assert(total(File_as_Reader(&f)) == 8);
    assert(total(Mock_as_Reader(&m)) == 4);
    assert(f.pos == 0);
    return 0;
}
