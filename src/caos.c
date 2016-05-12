#include <stdio.h>
#include <stdlib.h>

#include "common.h"

#include "RBuf.h"
#include "SBuf.h"

extern FILE* source;
extern FILE* public_header;
extern FILE* private_header;
extern FILE* define_header;

int bracket = 0;
boolean arg_comma = FALSE;
boolean isMember = FALSE;

SBuf* class_buf;
SBuf* obj_class_buf;
SBuf* word_buf;
SBuf* obj_buf;
char allque[MAX_BUF*MAX_TMP];
char* input_filename;
char filename[MAX_BUF];
char source_name[MAX_BUF];
char public_header_name[MAX_BUF];
char private_header_name[MAX_BUF];
char define_header_name[MAX_BUF];

RBuf* tmp;
boolean isLineNo = TRUE;
boolean isThisPointer = TRUE;
boolean isVerbose = FALSE;
visibility methodVisibility = VISIBILITY_NONE;
boolean isOldComment = FALSE;
boolean singleton = FALSE;
