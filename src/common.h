#define MAX_BUF (100)
#define MAX_TMP (50)

#include "boolean.h"
#include "buftype.h"

typedef enum _visibility {
	VISIBILITY_NONE, 
	VISIBILITY_PUBLIC, 
	VISIBILITY_PROTECTED, 
	VISIBILITY_PRIVATE 
} visibility;
