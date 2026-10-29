#include "Counter.h"
#include "Counter_define.h"
#include "Counter_private.h"

void Counter_add(Counter* pthis, int d){ pthis->n += d; }
int Counter_get(Counter* pthis){ return pthis->n; }

