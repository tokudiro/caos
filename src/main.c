#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "common.h"

#include "boolean.h"
#include "buftype.h"

#include "RBuf_define.h"
#include "SBuf_define.h"
#include "SLib.h"

/* flex extern */
extern FILE *yyin;
extern int yylex (void);

FILE* source;
FILE* public_header;
FILE* private_header;
FILE* define_header;
FILE* struct_header;

SBuf* class_buf;

extern RBuf* queue;
extern char filename[];
extern char source_name[];
extern char public_header_name[];
extern char private_header_name[];
extern char define_header_name[];

extern boolean isLineNo;
extern boolean isThisPointer;
extern boolean isVerbose;
extern char* input_filename;
extern boolean isOldComment;
extern classtype classType;
extern boolean isNonPrivateHeader;

extern int countPublicMethod;
extern int countPrivateMethod;
extern int countClassMethod;
extern int countAttribute;

static void outputHELP(){
	printf("------------------------------------------------------\n");
	printf("C-language Addtional Object-oriented Source Transpiler\n");
	printf("(option)\n");
	printf("h : HELP\n");
	printf("V : verbose\n");
	printf("L : LINENO OFF\n");
	printf("S : self object\n");
	printf("C : replace to old style comment\n");
}

extern char	*optarg;

int main(int argc, char** argv)
{
	int c;
	while( (c = getopt(argc, argv, "hVLSCo:p")) !=-1 ) {
		switch(c){
		case 'h':
		    outputHELP();
			exit(0);
			break;
		case 'V':
			isVerbose = TRUE;
			break;
		case 'L':
			isLineNo = FALSE;
			break;
		case 'S':
			isThisPointer = FALSE;
			break;
		case 'C':
			isOldComment = TRUE;
			break;
		case 'o':
		    printf("output :%s\n", optarg);
		    break;
		case 'p':
		    isNonPrivateHeader = TRUE;
		    break;
		default:
			break;
		}
	}
	
	RBuf queue_impl;
	queue = &queue_impl;
	RBuf_init(queue, isThisPointer, isVerbose);
	
	SBuf class_buf_impl;
	class_buf = &class_buf_impl;
	SBuf_init(class_buf);

	if(optind < argc) {
		input_filename = argv[optind];
		yyin = fopen(input_filename, "r");
	} else {
	    outputHELP();
	    exit(0);
	}

	sscanf(input_filename, "%[^.]]", filename);
	
	if (isVerbose){
    	printf("------------------------------------------------------\n");
    	printf("C-language Addtional Object-oriented Source Transpiler\n");
		printf("file: %s\n", input_filename);
    	printf("------------------------------------------------------\n");
		printf("Verbose Mode\n\n");
	}

	*source_name = 0;
	strcat(source_name, filename);
	strcat(source_name, ".c");
	source = fopen(source_name, "w");
	if (source == 0) {printf("source file error.");exit(-1);}

	*public_header_name = 0;
	strcat(public_header_name, filename);
	strcat(public_header_name, ".h");
	public_header = fopen(public_header_name, "w");
	if (public_header == 0) {printf("public header file error.");exit(-1);}

	*private_header_name = 0;
	strcat(private_header_name, filename);
	strcat(private_header_name, "_private.h");
    if (!isNonPrivateHeader) {
    	private_header = fopen(private_header_name, "w");
    	if (private_header == 0) {printf("private header file error.");exit(-1);}
    }

	*define_header_name = 0;
	strcat(define_header_name, filename);
	strcat(define_header_name, "_define.h");
	define_header = fopen(define_header_name, "w");
	if (define_header == 0) {printf("define header file error.");exit(-1);}
    struct_header = define_header;
    
    char guardname[MAX_TEXT] = {0};
    strcat(guardname, "__");
    SLib_toupper(guardname, filename);
    
	fprintf( public_header, "#ifndef __%s_H__\n", guardname );
	fprintf( public_header, "#define __%s_H__\n", guardname );
	int ret = yylex();
	fprintf( public_header, "#endif /* __%s_H__ */\n", guardname );

	fclose(source);
	fclose(public_header);
    if (!isNonPrivateHeader) {
        fclose(private_header);
    }
	fclose(define_header);
    struct_header = 0;

    if (isVerbose){
        printf("------------------------------------------------------\n");
        printf("PUBLIC  METHOD: %d\n", countPublicMethod);
        printf("PRIVATE METHOD: %d\n", countPrivateMethod);
        printf("CLASS   METHOD: %d\n", countClassMethod);
        printf("------------------------------------------------------\n");
        printf("ATTRIBUTE     : %d\n", countAttribute);
    }
    return ret;
}
