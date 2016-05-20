#include <stdio.h>
#include <stdlib.h>

#include "common.h"

#include "RBuf.h"
#include "SBuf.h"
#include "SLib.h"

extern FILE* source;
extern FILE* public_header;
extern FILE* private_header;
extern FILE* define_header;
extern FILE* struct_header;

int bracket = 0;
boolean arg_comma = FALSE;
boolean isMember = FALSE;

SBuf* class_buf;
char allque[MAX_TEXT*MAX_ARRAYSIZE];
char* input_filename;
char filename[MAX_TEXT];
char source_name[MAX_TEXT];
char public_header_name[MAX_TEXT];
char private_header_name[MAX_TEXT];
char define_header_name[MAX_TEXT];

RBuf* queue;
boolean isLineNo = TRUE;
boolean isThisPointer = TRUE;
boolean isVerbose = FALSE;
boolean isOldComment = FALSE;

visibility methodVisibility = VISIBILITY_NONE;
scope methodScope = SCOPE_OBJECT;
classtype classType = TYPE_CLASS;

int countPublicMethod = 0;
int countPrivateMethod = 0;
int countClassMethod = 0;
int countAttribute = 0;
