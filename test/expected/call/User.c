#include  "Counter.h"
#include "Counter_define.h"   // the struct is needed for a Counter value
#include "User.h"
#include "User_define.h"
#include "User_private.h"

int User_use(User* pthis, Counter* c, int d)
{
    Counter v;
    v.n = 100;
    Counter_add(c, d);
    Counter_add(&v, d);
    return Counter_get(c) + Counter_get(&v);
}

