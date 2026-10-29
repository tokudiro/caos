#ifndef COMMON_H__
#define COMMON_H__

#define MAX_TEXT (256)
#define MAX_ARRAYSIZE (256)
#define MAX_TEXTLINE (1024)
#define MAX_PATH (4096)
/* buffer for RBuf_allDeque : each of MAX_ARRAYSIZE elements (MAX_TEXT-1 chars)
   may get a prefix of up to MAX_TEXT+1 chars ("pthis->", "Class_") */
#define MAX_ALLQUE ((MAX_TEXT*2+8)*MAX_ARRAYSIZE)

#include "boolean.h"
#include "buftype.h"

typedef enum _visibility {
    VISIBILITY_NONE, 
    VISIBILITY_PUBLIC, 
    VISIBILITY_PROTECTED, 
    VISIBILITY_PRIVATE 
} visibility;

typedef enum _scope {
    SCOPE_OBJECT,
    SCOPE_CLASS,
    SCOPE_METHOD,
    SCOPE_GLOBAL
} scope;

typedef enum _classtype {
    TYPE_CLASS, 
    TYPE_SINGLETON, 
    TYPE_STRUCT,
    TYPE_UTILITY
} classtype;

typedef enum _mutability {
    MUTABILITY_MUTABLE,
    MUTABILITY_IMMUTABLE
} mutability;

#endif /* COMMON_H__ */
