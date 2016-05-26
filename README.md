# caos
C-language additional object-oriented source

## Description
object-oriented source transpiler for C-language.
It implements only my minimum necessary functions .

---
## Build
build environment : gcc + flex

``` ./build.sh ```

## Sample Code

* SampleClass.caos

```c
#include <stdio.h>
#class $SampleClass

#@ int publicMember;
int privateMember;

#+int publicMethod(void){
    @privateMethod();
    return @privateMember;
}

#-void privateMethod(){
    @privateMember = 2 + @publicMember;
    return;
}

#endclass

SampleClass obj_impl;

int main(){
    $SampleClass obj = &obj_impl;
    SampleClass_set_publicMember(obj, 1);
    printf("%d\n", SampleClass_get_publicMember(obj) );
    printf("%d\n", SampleClass_publicMethod(obj) );
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

int SampleClass_get_publicMember(const SampleClass* this) { return this->publicMember; }
void SampleClass_set_publicMember(SampleClass* this, int publicMember) { this->publicMember = publicMember; }

int SampleClass_publicMethod(SampleClass* this){
    SampleClass_privateMethod(this);
    return this->privateMember;
}

void SampleClass_privateMethod(SampleClass* this){
    this->privateMember = 2 + this->publicMember;
    return;
}

SampleClass obj_impl;

int main(){
    SampleClass* obj = &obj_impl;
    SampleClass_set_publicMember(obj, 1);
    printf("%d\n", SampleClass_get_publicMember(obj) );
    printf("%d\n", SampleClass_publicMethod(obj) );
}
```

* SampleClass.h

```c
#ifndef __SAMPLECLASS_H__
#define __SAMPLECLASS_H__
/* class SampleClass */
typedef struct SampleClass_struct SampleClass;
int SampleClass_get_publicMember(const SampleClass* this);
void SampleClass_set_publicMember(SampleClass* this, int publicMember);
int SampleClass_publicMethod(SampleClass* this);
#endif /* __SAMPLECLASS_H__ */
```

* SampleClass_define.h

```c
#include "SampleClass.h"
/* class SampleClass */
struct SampleClass_struct{
int publicMember;
int privateMember;
};
```

* SampleClass_private.h

```c
static void SampleClass_privateMethod(SampleClass* this);
```
