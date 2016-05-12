# caos
C-language additional object source

---

## Sample

* SampleClass.caos

```
#class $SampleClass

int member;

#+int public_method(void){
	return 0;
}

#-int private_method(void){
	return 0;
}

#endclass
```

generate files
```caos -L SampleClass.caos```

* SampleClass.c

```c
#include "SampleClass_define.h"
#include "SampleClass_private.h"
/* class SampleClass */

int SampleClass_public_method(SampleClass* this, void){
    return 0;
}

int SampleClass_private_method(SampleClass* this, void){
    return 0;
}
```

* SampleClass.h

```c
#ifndef __SampleClass_H__
#define __SampleClass_H__
/* class SampleClass */
typedef struct SampleClass_struct SampleClass;
int SampleClass_public_method(SampleClass* this, void);
#endif /* __SampleClass_H__ */
```

* SampleClass_define.h

```c
#include "SampleClass.h"
/* class SampleClass */
struct SampleClass_struct{
int member;
};
```

* SampleClass_private.h

```c
static int SampleClass_private_method(SampleClass* this, void);
```
