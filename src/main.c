#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "common.h"

#include "boolean.h"
#include "buftype.h"

#include "RBuf_define.h"
#include "SBuf_define.h"

/* flex extern */
extern FILE *yyin;
extern int yylex (void);

FILE* source;
FILE* public_header;
FILE* private_header;
FILE* define_header;

SBuf* class_buf;
SBuf* obj_class_buf;
SBuf* word_buf;
SBuf* obj_buf;

extern RBuf* tmp;
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
extern boolean singleton;

int main(int argc, char** argv)
{
	int c;
	while( (c = getopt(argc, argv, "hVLSC")) !=-1 ) {
		switch(c){
		case 'h':
			printf("--------------------\n");
			printf("C-language Addtional Object Source\n");
			printf("[[ option ]]\n");
			printf("h : HELP\n");
			printf("V : verbose\n");
			printf("[source style]");
			printf("L : LINENO OFF\n");
			printf("S : self object\n");
			printf("C : replace old comment\n");
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
		default:
			break;
		}
	}
	
	RBuf tmp_impl;
	tmp = &tmp_impl;
	RBuf_init(tmp, isThisPointer);
	
	SBuf class_buf_impl;
	class_buf = &class_buf_impl;
	SBuf_init(class_buf);

	SBuf obj_class_buf_impl;
	obj_class_buf = &obj_class_buf_impl;
	SBuf_init(obj_class_buf);

	SBuf word_buf_impl;
	word_buf = &word_buf_impl;
	SBuf_init(word_buf);

	SBuf obj_buf_impl;
	obj_buf = &obj_buf_impl;
	SBuf_init(obj_buf);

	if(optind < argc) {
		input_filename = argv[optind];
		yyin = fopen(input_filename, "r");
	} else {
		input_filename = "Temp.caos";
	}

	sscanf(input_filename, "%[^.]]", filename);
	
	if (isVerbose){
		printf("--------------------\n");
		printf("C-language Addtional Object Source\n");
		printf("file: %s\n", input_filename);
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
	private_header = fopen(private_header_name, "w");
	if (private_header == 0) {printf("private header file error.");exit(-1);}

	*define_header_name = 0;
	strcat(define_header_name, filename);
	strcat(define_header_name, "_define.h");
	define_header = fopen(define_header_name, "w");
	if (define_header == 0) {printf("define header file error.");exit(-1);}

	fprintf( public_header, "#ifndef __%s_H__\n", filename );
	fprintf( public_header, "#define __%s_H__\n", filename );
	int ret = yylex();
	fprintf( public_header, "#endif /* __%s_H__ */\n", filename );

	fclose(source);
	fclose(public_header);
	fclose(private_header);
	fclose(define_header);

	return ret;
}
