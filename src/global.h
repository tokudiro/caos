#ifndef GLOBAL_H__
#define GLOBAL_H__
/*
 * Global state shared between main.c and the lexer (Caos.c + *.l).
 * Each variable is defined in exactly one place (noted below).
 */
#include <stdio.h>

#include "common.h"
#include "RBuf.h"
#include "SBuf.h"
#include "Caos.h"

/* output files : defined in main.c */
extern FILE* source;
extern FILE* public_header;
extern FILE* private_header;
extern FILE* define_header;
extern FILE* struct_header;

/* defined in Caos.c */
extern SBuf* class_buf;
extern SBuf* thispointer_buf;
extern RBuf* queue;
extern Caos* caos;

extern char* input_filename;
extern char filename[];
extern char source_name[];
extern char public_header_name[];
extern char private_header_name[];
extern char define_header_name[];

extern boolean isLineNo;
extern boolean isVerbose;
extern boolean isOldComment;
extern boolean isNonPrivateHeader;
extern boolean isNonDefineHeader;
extern boolean isOutputHeader;
extern classtype classType;

extern int countPublicMethod;
extern int countPrivateMethod;
extern int countClassMethod;
extern int countAttribute;
extern int errorCount;

#endif /* GLOBAL_H__ */
