# caos
C-language additional object source

---
## Build
build environment : gcc + flex

``` ./build.sh ```

## Sample Code

* SampleClass.caos

```c
#include <stdio.h>
#class $SampleClass

#@ int public_member;
int private_member;

#+int public_method(void){
    SampleClass_private_method(this);
    return @private_member;
}

#-void private_method(){
    @private_member = 2 + @public_member;
    return;
}

#endclass

SampleClass obj_impl;

int main(){
    $SampleClass obj = &obj_impl;
    SampleClass_set_public_member(obj, 1);
    printf("%d\n", SampleClass_get_public_member(obj) );
    printf("%d\n", SampleClass_public_method(obj) );
}
```

## Generate files
```caos -L SampleClass.caos```

## Sample Output
* SampleClass.c

```c
#include <stdio.h>

#include "SampleClass_define.h"
#include "SampleClass_private.h"
/* class SampleClass */

int SampleClass_get_public_member(const SampleClass* this) { return this->public_member; }
void SampleClass_set_public_member(SampleClass* this, int public_member) { this->public_member = public_member; }

int SampleClass_public_method(SampleClass* this){
    SampleClass_private_method(this);
    return this->private_member;
}

void SampleClass_private_method(SampleClass* this){
    this->private_member = 2 + this->public_member;
    return;
}

SampleClass obj_impl;

int main(){
    SampleClass* obj = &obj_impl;
    SampleClass_set_public_member(obj, 1);
    printf("%d\n", SampleClass_get_public_member(obj) );
    printf("%d\n", SampleClass_public_method(obj) );
}
```

* SampleClass.h

```c
#ifndef __SampleClass_H__
#define __SampleClass_H__
/* class SampleClass */
typedef struct SampleClass_struct SampleClass;
int SampleClass_get_public_member(const SampleClass* this);
void SampleClass_set_public_member(SampleClass* this, int public_member);
int SampleClass_public_method(SampleClass* this);
#endif /* __SampleClass_H__ */
```

* SampleClass_define.h

```c
#include "SampleClass.h"
/* class SampleClass */
struct SampleClass_struct{
 int public_member;
int private_member;
};
```

* SampleClass_private.h

```c
static void SampleClass_private_method(SampleClass* this);
```
