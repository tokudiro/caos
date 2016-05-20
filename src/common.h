#define MAX_TEXT (256)
#define MAX_ARRAYSIZE (256)

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

