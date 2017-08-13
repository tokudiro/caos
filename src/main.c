#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "common.h"

#include "boolean.h"
#include "buftype.h"

#include "RBuf.h"
#include "RBuf_define.h"
#include "ListBuf.h"
#include "SBuf.h"
#include "SBuf_define.h"
#include "SLib.h"
#include "Caos.h"

/* flex extern */
extern FILE *yyin;
extern int yylex (void);

FILE* source;
FILE* public_header;
FILE* private_header;
FILE* define_header;
FILE* struct_header;

SBuf* class_buf;
SBuf* thispointer_buf;

extern RBuf* queue;
extern Caos* caos;

extern char filename[];
extern char source_name[];
extern char public_header_name[];
extern char private_header_name[];
extern char define_header_name[];

extern boolean isLineNo;
extern boolean isVerbose;
extern char* input_filename;
extern boolean isOldComment;
extern classtype classType;
extern boolean isNonPrivateHeader;
extern boolean isNonDefineHeader;
extern boolean isOutputHeader;

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
	printf("C : replace to old style comment\n");
    printf("k : change thispointer string\n");
}

extern char	*optarg;

static const char default_thispointer_str[] = "pthis";

int main(int argc, char** argv)
{
	SBuf thispointer_buf_impl;
    RBuf queue_impl;
	SBuf class_buf_impl;
    int c;

    thispointer_buf = &thispointer_buf_impl;
	SBuf_init(thispointer_buf);

	while( (c = getopt(argc, argv, "hVLCo:pdk:")) !=-1 ) {
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
		case 'C':
			isOldComment = TRUE;
			break;
		case 'o':
		    printf("output :%s\n", optarg);
		    break;
		case 'p':
		    isNonPrivateHeader = TRUE;
		    break;
		case 'd':
		    isNonDefineHeader = TRUE;
		    break;
		case 'k':
		    SBuf_setBuf(thispointer_buf, optarg, strlen(optarg));
		    break;
		default:
			break;
		}
	}
	
    if ( SBuf_getLen(thispointer_buf) == 0 ) {
        SBuf_setBuf(thispointer_buf, default_thispointer_str, strlen(default_thispointer_str) );
    }

	queue = &queue_impl;
	RBuf_init(queue, SBuf_getStr(thispointer_buf), isVerbose);
	
    caos = Caos_getInstance();
    Caos_init(caos);
    
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
        printf("thispointer name : %s\n", SBuf_getStr(thispointer_buf));
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
    } else {
        private_header = 0;
    }

	*define_header_name = 0;
	strcat(define_header_name, filename);
	strcat(define_header_name, "_define.h");
    if (!isNonDefineHeader) {
	    define_header = fopen(define_header_name, "w");
	    if (define_header == 0) {printf("define header file error.");exit(-1);}
        struct_header = define_header;
    } else {
        define_header = 0;
        struct_header = 0;
    }
    
    char guardname[MAX_TEXT] = {0};
    SLib_toupper(guardname, filename);
    
	fprintf( public_header, "#ifndef %s_H__\n", guardname );
	fprintf( public_header, "#define %s_H__\n", guardname );
	int ret = yylex();
    
    if (!isNonDefineHeader) {
        fclose(define_header);
    }
    if (classType == TYPE_STRUCT) {
        if (!isNonDefineHeader) {
            define_header = fopen(define_header_name, "r");
            if (define_header == 0) {printf("define header file error.");exit(-1);}
            
            char readline[MAX_TEXTLINE];
            while (fgets(readline, MAX_TEXTLINE, define_header) != NULL) {
                fprintf( public_header, "%s", readline );
            }
            fclose(define_header);
            if (remove(define_header_name) != 0) {printf("define header file error.");exit(-1);}
        }
    }
	fprintf( public_header, "#endif /* %s_H__ */\n", guardname );

	fclose(source);
	fclose(public_header);
    if (!isNonPrivateHeader) {
        fclose(private_header);
    }
    struct_header = 0;

    if (isVerbose){
        printf("------------------------------------------------------\n");
        printf("PUBLIC  METHOD: %d\n", countPublicMethod);
        printf("PRIVATE METHOD: %d\n", countPrivateMethod);
        printf("CLASS   METHOD: %d\n", countClassMethod);
        printf("------------------------------------------------------\n");
        printf("ATTRIBUTE     : %d\n", countAttribute);
        printf("------------------------------------------------------\n");
        printf("list.length   : %d\n", ListBuf_length(Caos_list(caos)) );
    }
    
    ListBuf_finish( Caos_list(caos) );
    
    return ret;
}
