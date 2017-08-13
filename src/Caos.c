#include <stdio.h>
#include <stdlib.h>

#include "common.h"


#include "RBuf.h"
#include "SBuf.h"
#include "SLib.h"

#include "Caos.h"
#include "ListBuf.h"
#include "Element.h"

extern FILE* source;
extern FILE* public_header;
extern FILE* private_header;
extern FILE* define_header;
extern FILE* struct_header;

int bracket = 0;
boolean arg_comma = FALSE;
boolean isMember = FALSE;

SBuf* class_buf;
SBuf* thispointer_buf;
char allque[MAX_TEXT*MAX_ARRAYSIZE];
char* input_filename;
char filename[MAX_TEXT];
char source_name[MAX_TEXT];
char public_header_name[MAX_TEXT];
char private_header_name[MAX_TEXT];
char define_header_name[MAX_TEXT];

RBuf* queue;
Caos* caos;

boolean isLineNo = TRUE;
boolean isVerbose = FALSE;
boolean isOldComment = FALSE;
boolean isNonPrivateHeader = FALSE;
boolean isNonDefineHeader = FALSE;
boolean isOutputHeader = FALSE;

visibility methodVisibility = VISIBILITY_NONE;
scope methodScope = SCOPE_OBJECT;
mutability methodMutability = MUTABILITY_MUTABLE;
classtype classType = TYPE_CLASS;

int countPublicMethod = 0;
int countPrivateMethod = 0;
int countClassMethod = 0;
int countAttribute = 0;

#include "Caos_define.h"
static struct Caos_struct instance;
Caos* Caos_getInstance() { return &instance; }


/**
* \brief initial caos
*/

void Caos_init(Caos* pthis)
{
    pthis->list = ListBuf_getInstance();
}


ListBuf* Caos_list(Caos* pthis)
{
    return pthis->list;
}


