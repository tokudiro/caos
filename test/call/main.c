#include <assert.h>
#include "Counter.h"
#include "Counter_define.h"
#include "User.h"
#include "User_define.h"

int main(void)
{
    Counter c = {1};
    User u;
    assert(User_use(&u, &c, 5) == 6 + 105);
    return 0;
}
