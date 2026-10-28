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
#include "global.h"

/* flex extern */
extern FILE *yyin;
extern int yylex (void);

FILE* source;
FILE* public_header;
FILE* private_header;
FILE* define_header;
FILE* struct_header;

static void outputHELP(){
	printf("------------------------------------------------------\n");
	printf("C-language Addtional Object-oriented Source Transpiler\n");
	printf("(option)\n");
	printf("h : HELP\n");
	printf("V : verbose\n");
	printf("L : LINENO OFF\n");
	printf("C : replace to old style comment\n");
    printf("k : change thispointer string\n");
    printf("p : do not output private header\n");
    printf("d : do not output define header\n");
    printf("o : output folder\n");
}

extern char	*optarg;

static const char default_thispointer_str[] = "pthis";

/* -o option: output folder (default: the folder of the input file) */
static const char* output_dir = 0;

/* path of "<dir>/<filename><suffix>" ; dir may be empty */
static void makePath(char* path, const char* dir, const char* suffix) {
    path[0] = 0;
    if (dir[0] != 0) {
        strcat(path, dir);
        if (dir[strlen(dir)-1] != '/') strcat(path, "/");
    }
    strcat(path, filename);
    strcat(path, suffix);
}

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
		    output_dir = optarg;
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

	/* filename : the input file name without folder and extension */
	char input_dir[MAX_TEXT] = {0};
	const char* slash = strrchr(input_filename, '/');
	const char* base = slash ? slash + 1 : input_filename;
	if (slash) {
		strncpy(input_dir, input_filename, slash - input_filename + 1);
	}
	strncpy(filename, base, MAX_TEXT - 1);
	char* dot = strrchr(filename, '.');
	if (dot) *dot = 0;
	const char* dir = output_dir ? output_dir : input_dir;
	
	if (isVerbose){
        printf("------------------------------------------------------\n");
        printf("C-language Addtional Object-oriented Source Transpiler\n");
        printf("file: %s\n", input_filename);
        printf("------------------------------------------------------\n");
        printf("Verbose Mode\n\n");
        printf("thispointer name : %s\n", SBuf_getStr(thispointer_buf));
	}

	/* names used in #include (same folder as the including file) */
	makePath(source_name, "", ".c");
	makePath(public_header_name, "", ".h");
	makePath(private_header_name, "", "_private.h");
	makePath(define_header_name, "", "_define.h");

	/* paths to open */
	char source_path[MAX_TEXT*2];
	char public_header_path[MAX_TEXT*2];
	char private_header_path[MAX_TEXT*2];
	char define_header_path[MAX_TEXT*2];
	makePath(source_path, dir, ".c");
	makePath(public_header_path, dir, ".h");
	makePath(private_header_path, dir, "_private.h");
	makePath(define_header_path, dir, "_define.h");

	source = fopen(source_path, "w");
	if (source == 0) {printf("source file error: %s\n", source_path);exit(-1);}

	public_header = fopen(public_header_path, "w");
	if (public_header == 0) {printf("public header file error: %s\n", public_header_path);exit(-1);}

    if (!isNonPrivateHeader) {
    	private_header = fopen(private_header_path, "w");
    	if (private_header == 0) {printf("private header file error: %s\n", private_header_path);exit(-1);}
    } else {
        private_header = 0;
    }

    if (!isNonDefineHeader) {
	    define_header = fopen(define_header_path, "w");
	    if (define_header == 0) {printf("define header file error: %s\n", define_header_path);exit(-1);}
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
            define_header = fopen(define_header_path, "r");
            if (define_header == 0) {printf("define header file error.");exit(-1);}
            
            char readline[MAX_TEXTLINE];
            while (fgets(readline, MAX_TEXTLINE, define_header) != NULL) {
                fprintf( public_header, "%s", readline );
            }
            fclose(define_header);
            if (remove(define_header_path) != 0) {printf("define header file error.");exit(-1);}
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
