%{
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

SBuf class_buf;
SBuf obj_class_buf;
SBuf word_buf;
SBuf obj_buf;
char allque[MAX_BUF*MAX_TMP];
char* input_filename;
char filename[MAX_BUF];
char source_name[MAX_BUF];
char public_header_name[MAX_BUF];
char private_header_name[MAX_BUF];
char define_header_name[MAX_BUF];

RBuf tmp;
boolean isLineNo = TRUE;
boolean isThisPointer = TRUE;
boolean isVerbose = FALSE;
visibility methodVisibility = VISIBILITY_NONE;
boolean isOldComment = FALSE;
boolean singleton = FALSE;
%}
NONZERO  [1-9]
ALPHABET [a-zA-Z_]
MOJI     [a-zA-Z_1-9]
COMMENTS (\/\*)
COMMENTE (\*\/)
COMMENT  (\/\/)
DIGIT    [[:digit:]]
FLOAT    ({NONZERO}{DIGIT}*\.?|0\.|\.{DIGIT}){DIGIT}*|0
FLOATE   [fF]

DIRECTIVE_IMPORT   ^#import
DIRECTIVE_CLASS    ^#class
DIRECTIVE_ENDCLASS ^#endclass
DIRECTIVE_SINGLETON ^#singleton

DIRECTIVE_PUBLIC_METHOD    ^#\+
DIRECTIVE_PRIVATE_METHOD   ^#-
DIRECTIVE_PROTECTED_METHOD ^##

DIRECTIVE_PUBLIC_ATTRIBUTE ^#\@

NUMBER   {FLOAT}{FLOATE}?
WSPACE   [[:blank:]]+
WORD     ({ALPHABET}{MOJI}*)
CLASS    (\${WORD})
OBJECT   (\@{WORD})
SELF_OBJECT  (\@\@)
DOT      (\.)

%option yylineno

%s IMPORT
%s CLASS_DEFINE
%s INITIAL_CSA
%s INITIAL_CSS
%s INITIAL_ST

%s CLASS_SCOPE
%s CLASS_SCOPE_CSA
%s CLASS_SCOPE_CSS

%s METHOD_DEFINE
%s METHOD_DEFINE_CSA
%s METHOD_DEFINE_CSS

%s METHOD_SCOPE
%s METHOD_SCOPE_CSA
%s METHOD_SCOPE_CSS
%s METHOD_SCOPE_ST

%s ATTRIBUTE_DEFINE
%s ATTRIBUTE_DEFINE_CSA
%s ATTRIBUTE_DEFINE_CSS
%%
<INITIAL>{
	{CLASS}  {
		fprintf(source, "%s", &(yytext[1]));
		if (isThisPointer) fprintf(source, "*");
	}

	{NUMBER} { fprintf(source, "%s", yytext); }

	{OBJECT} { fprintf(source, "%s", &(yytext[1])); }
	{WORD}   { fprintf(source, "%s", yytext); }
	{WSPACE} { fprintf(source, "%s", yytext); }
	{COMMENTS} { fprintf(source, "/*"); BEGIN(INITIAL_CSA); }
	{COMMENT}  {
		if (isOldComment) {
			fprintf(source, "/*"); BEGIN(INITIAL_CSS);
		} else {
			fprintf(source, "//"); BEGIN(INITIAL_CSS);
		}
	}
	\"  { fprintf( source, "\""); BEGIN(INITIAL_ST); }
  
	{DIRECTIVE_IMPORT} {
		fprintf(source, "#include ");
		fprintf(define_header, "#include ");
		BEGIN(IMPORT);
	}
	{DIRECTIVE_CLASS} { BEGIN(CLASS_DEFINE); }
	{DIRECTIVE_SINGLETON} {
		singleton = TRUE;
		BEGIN(CLASS_DEFINE);
	}
	
	\n { fprintf(source, "\n"); }
	.  { fprintf(source, "%s", yytext); }
}

<INITIAL_CSA>{
	{COMMENTE} { fprintf(source, "*/"); BEGIN(INITIAL); }
	. { fprintf(source, "%s", yytext); }
}

<INITIAL_CSS>{
	\n {
		if (isOldComment) {
			fprintf(source, "*/\n"); BEGIN(INITIAL);
		} else {
			fprintf(source, "\n"); BEGIN(INITIAL);
		}
	}
	. { fprintf(source, "%s", yytext); }
}

<INITIAL_ST>{
	\\\" { fprintf( source, "\\\""); }
	\" { fprintf( source, "\""); BEGIN(INITIAL); }
	. { fprintf( source, "%s", yytext); }
}
<IMPORT>{
	{CLASS}  {
		fprintf(source, "\"%s.h\"\n", &(yytext[1]));
		fprintf(define_header, "\"%s.h\"\n", &(yytext[1]));
		BEGIN(INITIAL);
	}

	{WSPACE} {
		fprintf(source, "%s", yytext);
		fprintf(define_header, "%s", yytext);
	}

	\n {
		fprintf(source, "\n");
		fprintf(define_header, "\n");
	}
	.  {
		fprintf(source, "%s", yytext);
		fprintf(define_header, "%s", yytext);
	}
}
%{
	void class_define()
	{
		if (isVerbose) printf("class %s\n", SBuf_getStr(class_buf) );
		fprintf( source, "#include \"%s\"\n", define_header_name );
		fprintf( source, "#include \"%s\"\n", private_header_name );
		fprintf( source, "/* class %s */\n", SBuf_getStr(class_buf) );
		if (singleton) {
			fprintf( source, "static struct %s_struct instance;\n", SBuf_getStr(class_buf) );
			fprintf( source, "%s", SBuf_getStr(class_buf) );
			if (isThisPointer) fprintf( source, "*");
			fprintf( source, " %s_getInstance() { return &instance; }\n", SBuf_getStr(class_buf) );
		}
		if (isLineNo) fprintf(source, "#line %d \"%s\"\n", yylineno, input_filename);

		fprintf( public_header, "/* class %s */\n", SBuf_getStr(class_buf) );
		if (isLineNo) fprintf(public_header, "#line %d \"%s\"\n", yylineno, input_filename);
		fprintf( public_header, "typedef struct %s_struct", SBuf_getStr(class_buf) );
		if (!isThisPointer) fprintf( public_header, "*");
		fprintf( public_header, " %s;\n", SBuf_getStr(class_buf) );
		if (singleton) {
			fprintf( public_header, "%s", SBuf_getStr(class_buf) );
			if (isThisPointer) fprintf( public_header, "*");
			fprintf( public_header, " %s_getInstance();\n", SBuf_getStr(class_buf) );
		}

		fprintf( define_header, "#include \"%s\"\n", public_header_name );
		fprintf( define_header, "/* class %s */\n", SBuf_getStr(class_buf) );
		if (isLineNo) fprintf(define_header, "#line %d \"%s\"\n", yylineno, input_filename);
		fprintf( define_header, "struct %s_struct{\n", SBuf_getStr(class_buf) );
	}
%}

<CLASS_DEFINE>{
	{WORD}  {
		SBuf_setBuf( class_buf, yytext, yyleng);
		class_define();
		BEGIN(CLASS_SCOPE);
	}
	
	{CLASS}  {
		SBuf_setBuf( class_buf, &(yytext[1]), yyleng-1);
		class_define();
		BEGIN(CLASS_SCOPE);
	}

	\n { fprintf( source, "\n" ); }
}
<CLASS_SCOPE>{
	{NUMBER} {
		RBuf_enque(tmp, yytext, yyleng, T_NUMBER);
	}

	{CLASS}  {
		//if (arg_comma) { RBuf_enque(tmp, ", ", 2, T_NON); arg_comma=FALSE; }
		RBuf_enque(tmp, &(yytext[1]), yyleng-1, T_CLASS);
		if(isThisPointer) RBuf_enque(tmp, "*", 1, T_KEYWORD);
	}

	{DIRECTIVE_PUBLIC_METHOD} {
		if (isVerbose) printf("public method\n");
		if (isLineNo) fprintf(source, "#line %d \"%s\"\n", yylineno, input_filename);
		if (isLineNo) fprintf(public_header, "#line %d \"%s\"\n", yylineno, input_filename);
		methodVisibility = VISIBILITY_PUBLIC;
		BEGIN(METHOD_DEFINE);
	}
	{DIRECTIVE_PRIVATE_METHOD} {
		if (isVerbose) printf("private method\n");
		if (isLineNo) fprintf(source, "#line %d \"%s\"\n", yylineno, input_filename);
		if (isLineNo) fprintf(private_header, "#line %d \"%s\"\n", yylineno, input_filename);
		fprintf(private_header, "static ");
		methodVisibility = VISIBILITY_PRIVATE;
		BEGIN(METHOD_DEFINE);
	}
	{DIRECTIVE_PROTECTED_METHOD} {
		//if (isLineNo) fprintf(source, "#line %d \"%s\"\n", yylineno, input_filename);
		//if (isLineNo) fprintf(protected_header, "#line %d \"%s\"\n", yylineno, input_filename);
		//methodVisibility = VISIBILITY_PROTECTED;
		//BEGIN(METHOD_DEFINE);
	}
	
	{DIRECTIVE_PUBLIC_ATTRIBUTE} {
		BEGIN(ATTRIBUTE_DEFINE);
	}
	
	\{ {
		char* method = RBuf_allque(tmp, allque, SBuf_getStr(class_buf));
		if (strlen(method) != 0) {
			fprintf( public_header, "%s;\n", method );
		}
		fprintf( source, "%s{", method );
		BEGIN(METHOD_SCOPE);
	}

	{DIRECTIVE_ENDCLASS} {
		fprintf( define_header, "};\n" );
		SBuf_init(class_buf);
		BEGIN(INITIAL);
	}

	\( {
		//boolean isMethod = RBuf_back_retype(tmp, T_WORD, T_METHOD);
		RBuf_enque( tmp, yytext, yyleng, T_NON);
		//if (isMethod) {
		//	RBuf_enque( tmp, SBuf_getStr(class_buf), SBuf_getLen(class_buf), T_CLASS);
		//	RBuf_enque( tmp, " self", 5, T_KEYWORD);
		//	arg_comma = TRUE;
		//}
	}

	\) {
		arg_comma = FALSE;
		RBuf_enque( tmp, ")", 1, T_NON);
	}

	{OBJECT} { RBuf_enque(tmp, &(yytext[1]), yyleng-1, T_OBJECT); }
	{WORD}   {
		//if (arg_comma) { RBuf_enque(tmp, ", ", 2, T_NON); arg_comma=FALSE; }
		RBuf_enque(tmp, yytext, yyleng, T_WORD);
	}
	{WSPACE}   { RBuf_enque(tmp, yytext, yyleng, T_NON); }
	{COMMENTS} {
		fprintf( (isMember?define_header:source), "/*");
		BEGIN(CLASS_SCOPE_CSA);
	}
	{COMMENT}  {
		if (isOldComment) {
			fprintf( (isMember?define_header:source), "/*");
		} else {
			fprintf( (isMember?define_header:source), "//");
		}
		BEGIN(CLASS_SCOPE_CSS);
	}
	\n {
		fprintf( (isMember?define_header:source), "%s", RBuf_allque(tmp, allque, SBuf_getStr(class_buf)) );
		fprintf( (isMember?define_header:source), "\n");
		if (isMember) { isMember = FALSE; }
	}
	;  {
		/* Class Member */
		fprintf(define_header, "%s;", RBuf_allque(tmp, allque, SBuf_getStr(class_buf)) );
		isMember = TRUE;
	}
	
	=  {
		/* TODO : bad case : struct member initialize */
		if (isVerbose) printf("error:struct member initialize\n");
		/* Class Member */
		fprintf(define_header, "%s;", RBuf_allque(tmp, allque, SBuf_getStr(class_buf)) );
		isMember = TRUE;
	}
	.  { RBuf_enque(tmp, yytext, yyleng, T_NON); }
}

<CLASS_SCOPE_CSA>{
	{COMMENTE} { fprintf( (isMember?define_header:source), "*/"); BEGIN(CLASS_SCOPE); }
	\n { fprintf( (isMember?define_header:source), "\n"); }
	.  { fprintf( (isMember?define_header:source), "%s", yytext); }
}

<CLASS_SCOPE_CSS>{
	\n {
		if (isOldComment) {
			fprintf((isMember?define_header:source), "*/\n");
		} else {
			fprintf((isMember?define_header:source), "\n");
		}
		BEGIN(CLASS_SCOPE);
	}
	.  { fprintf((isMember?define_header:source), "%s", yytext); }
}
<METHOD_DEFINE>{
	{NUMBER} {
		RBuf_enque(tmp, yytext, yyleng, T_NUMBER);
	}

	{CLASS}  {
		if (arg_comma) { RBuf_enque(tmp, ", ", 2, T_NON); arg_comma=FALSE; }
		RBuf_enque(tmp, &(yytext[1]), yyleng-1, T_CLASS);
		if (isThisPointer) RBuf_enque(tmp, "*", 1, T_KEYWORD);
	}

	\{ {
		char* method = RBuf_allque(tmp, allque, SBuf_getStr(class_buf));
		switch(methodVisibility){
		case VISIBILITY_PUBLIC:
			fprintf( public_header, "%s;\n", method );
			break;
		case VISIBILITY_PRIVATE:
			fprintf( private_header, "%s;\n", method );
			break;
		default:
			break;
		}
		fprintf( source, "%s{", method );
		BEGIN(METHOD_SCOPE);
	}

	\( {
		boolean isMethod = RBuf_back_retype(tmp, T_WORD, T_METHOD);
		RBuf_enque( tmp, yytext, yyleng, T_NON);
		if (isMethod) {
			RBuf_enque( tmp, SBuf_getStr(class_buf), SBuf_getLen(class_buf), T_CLASS);
			if (isThisPointer){
				RBuf_enque( tmp, "* this", 6, T_KEYWORD);
			}else{
				RBuf_enque( tmp, " self", 5, T_KEYWORD);
			}
			arg_comma = TRUE;
		}
	}

	\) {
		arg_comma = FALSE;
		RBuf_enque( tmp, ")", 1, T_NON);
	}

	{OBJECT} { RBuf_enque(tmp, &(yytext[1]), yyleng-1, T_OBJECT); }
	{WORD}   {
		if (arg_comma) { RBuf_enque(tmp, ", ", 2, T_NON); arg_comma=FALSE; }
		RBuf_enque(tmp, yytext, yyleng, T_WORD);
	}
	{WSPACE}   { RBuf_enque(tmp, yytext, yyleng, T_NON); }
	{COMMENTS} { fprintf( source, "/*"); BEGIN(METHOD_DEFINE_CSA); }
	{COMMENT}  {
		if (isOldComment) {
			fprintf( source, "/*");
		} else {
			fprintf( source, "//");
		}
		BEGIN(METHOD_DEFINE_CSS);
	}
	\n {
		fprintf( source, "%s\n", RBuf_allque(tmp, allque, SBuf_getStr(class_buf)) );
	}
	.  { RBuf_enque(tmp, yytext, yyleng, T_NON); }
}

<METHOD_DEFINE_CSA>{
	{COMMENTE} { fprintf( (source), "*/"); BEGIN(METHOD_DEFINE); }
	\n { fprintf( source, "\n"); }
	.  { fprintf( source, "%s", yytext); }
}

<METHOD_DEFINE_CSS>{
	\n {
		if (isOldComment) {
			fprintf( source, "*/\n");
		} else {
			fprintf( source, "\n");
		}
		BEGIN(METHOD_DEFINE);
	}
	.  { fprintf( source, "%s", yytext); }
}
<METHOD_SCOPE>{
	{NUMBER} {
		fprintf( source, "%s%s", RBuf_allque(tmp, allque, SBuf_getStr(class_buf)), yytext);
	}

	{CLASS}  {
		fprintf(source, "%s", &(yytext[1]));
		SBuf_setBuf( obj_class_buf, &(yytext[1]), yyleng-1);
	}

	\{ {
		fprintf( source, "%s{", RBuf_allque(tmp, allque, SBuf_getStr(class_buf)) );
		bracket++;
	}

	\} {
		fprintf( source, "%s}", RBuf_allque(tmp, allque, SBuf_getStr(class_buf)) );
		if (bracket == 0) {
			methodVisibility = VISIBILITY_NONE;
			BEGIN(CLASS_SCOPE);
		} else {
			bracket--;
		}
	}

	\( {
		fprintf( source, "%s(", RBuf_allque(tmp, allque, SBuf_getStr(class_buf)) );
	}

	, {
		fprintf( source, "%s,", RBuf_allque(tmp, allque, SBuf_getStr(class_buf)) );
	}

	\) {
		fprintf( source, "%s%s", RBuf_allque(tmp, allque, SBuf_getStr(class_buf)), yytext);
	}

	{OBJECT} {
		fprintf( source, "%s", RBuf_allque(tmp, allque, SBuf_getStr(class_buf)) );
		RBuf_enque(tmp, &(yytext[1]), yyleng-1, T_OBJECT);
		SBuf_setBuf( word_buf, &(yytext[1]), yyleng-1);
	}
	{SELF_OBJECT} {
		fprintf( source, "%s", RBuf_allque(tmp, allque, SBuf_getStr(class_buf)) );
		if (isThisPointer) {
			RBuf_enque(tmp, "this", 4, T_WORD);
		}else{
			RBuf_enque(tmp, "self", 4, T_WORD);
		}
	}
	{WORD}   {
		RBuf_enque(tmp, yytext, yyleng, T_WORD);
	}
	{WSPACE} {
		if ( RBuf_empty(tmp) == TRUE ) {
			fprintf(source, "%s", yytext);
		} else {
			RBuf_enque(tmp, yytext, yyleng, T_NON);
		}
	}
	{COMMENTS} { fprintf( source, "/*"); BEGIN(METHOD_SCOPE_CSA); }
	{COMMENT}  {
		if (isOldComment) {
			fprintf( source, "/*"); BEGIN(METHOD_SCOPE_CSS);
		} else {
			fprintf( source, "//"); BEGIN(METHOD_SCOPE_CSS);
		}
	}
	\"  { fprintf( source, "\""); BEGIN(METHOD_SCOPE_ST); }
	{DOT} {
		// [TODO] object table check
		SBuf_setBuf( obj_buf, SBuf_getStr(word_buf), SBuf_getLen(word_buf));
		RBuf_enque(tmp, yytext, yyleng, T_DOT);
	}
	\n { fprintf( source, "\n"); }
	;  {
		fprintf( source, "%s;", RBuf_allque(tmp, allque, SBuf_getStr(class_buf)) );
	}
	:  {
		fprintf( source, "%s:", RBuf_allque(tmp, allque, SBuf_getStr(class_buf)) );
	}
	.  { RBuf_enque(tmp, yytext, yyleng, T_NON); }
}

<METHOD_SCOPE_CSA>{
	{COMMENTE} { fprintf(source, "*/"); BEGIN(METHOD_SCOPE); }
	. { fprintf( source, "%s", yytext); }
}

<METHOD_SCOPE_CSS>{
	\n {
		if (isOldComment) {
			fprintf( source, "*/\n"); BEGIN(METHOD_SCOPE);
		} else {
			fprintf( source, "\n"); BEGIN(METHOD_SCOPE);
		}
	}
	. { fprintf( source, "%s", yytext); }
}

<METHOD_SCOPE_ST>{
	\\\" { fprintf( source, "\\\""); }
	\" { fprintf( source, "\""); BEGIN(METHOD_SCOPE); }
	. { fprintf( source, "%s", yytext); }
}
<ATTRIBUTE_DEFINE>{
	{NUMBER} {
		RBuf_enque(tmp, yytext, yyleng, T_NUMBER);
	}

	{CLASS}  {
		//if (arg_comma) { RBuf_enque(tmp, ", ", 2, T_NON); arg_comma=FALSE; }
		RBuf_enque(tmp, &(yytext[1]), yyleng-1, T_CLASS);
		if(isThisPointer) RBuf_enque(tmp, "*", 1, T_ASTERISK);
	}

	{DIRECTIVE_ENDCLASS} {
		fprintf( define_header, "};\n" );
		SBuf_init(class_buf);
		BEGIN(INITIAL);
	}

	{OBJECT} { RBuf_enque(tmp, &(yytext[1]), yyleng-1, T_OBJECT); }
	{WORD}   {
		RBuf_enque(tmp, yytext, yyleng, T_WORD);
	}
	{WSPACE}   { RBuf_enque(tmp, yytext, yyleng, T_NON); }
	{COMMENTS} { fprintf( define_header, "/*"); BEGIN(ATTRIBUTE_DEFINE_CSA); }
	{COMMENT}  {
		if (isOldComment) {
			fprintf( define_header, "/*");
		} else {
			fprintf( define_header, "//");
		}
		BEGIN(ATTRIBUTE_DEFINE_CSS);
	}
	\n {
		fprintf( define_header, "%s", RBuf_allque(tmp, allque, SBuf_getStr(class_buf)) );
		fprintf( define_header, "\n");
		if (isMember) { isMember = FALSE; }
		BEGIN(CLASS_SCOPE);
	}
	;  {
		/* Class Member */
		isMember = RBuf_back_retype(tmp, T_WORD, T_PARAMETER);
		boolean isType = RBuf_back_retype(tmp, T_WORD, T_TYPE);
		char* clazz = RBuf_back_getStr(tmp, T_CLASS);
		if ( 0!= clazz ) isType = TRUE;
		if (isVerbose) printf("RBuf_toString :\n%s\n", RBuf_toString(tmp, allque) );
		if (isMember && isType) {
			char* member = RBuf_back_getStr(tmp, T_PARAMETER);
			char* asterisk = RBuf_back_getStr(tmp, T_ASTERISK);
			char* type = RBuf_back_getStr(tmp, T_TYPE);
			if (isVerbose) printf("member : %s\n", member);
			
			// get header
			if (0!=clazz) {
				fprintf(public_header, "%s", clazz);
			} else {
				fprintf(public_header, "%s", type);
			}
			if (0 != asterisk) fprintf(public_header, "*");
			fprintf(public_header, " %s_get_%s(", SBuf_getStr(class_buf), member);
			if (isThisPointer){
				fprintf(public_header, "const %s* this", SBuf_getStr(class_buf) );
			}else{
				fprintf(public_header, "%s self", SBuf_getStr(class_buf) );
			}
			fprintf(public_header, ");\n");
			
			// get source
			if (0!=clazz) {
				fprintf(source, "%s", clazz);
			} else {
				fprintf(source, "%s", type);
			}
			if (0 != asterisk) fprintf(source, "*");
			fprintf(source, " %s_get_%s(", SBuf_getStr(class_buf), member);
			if (isThisPointer){
				fprintf(source, "const %s* this", SBuf_getStr(class_buf) );
			}else{
				fprintf(source, "%s self", SBuf_getStr(class_buf) );
			}
			fprintf(source, ") { return ");
			if (isThisPointer){
				fprintf(source, "this->%s", member );
			}else{
				fprintf(source, "self->%s", member );
			}
			fprintf(source, ";}\n");
			
		}
		fprintf(define_header, "%s;", RBuf_allque(tmp, allque, SBuf_getStr(class_buf)) );
	}
	\*  { RBuf_enque(tmp, yytext, yyleng, T_ASTERISK); }
	.  { RBuf_enque(tmp, yytext, yyleng, T_NON); }
}

<ATTRIBUTE_DEFINE_CSA>{
	{COMMENTE} {
		fprintf( define_header, "*/");
		BEGIN(ATTRIBUTE_DEFINE);
	}
	\n { fprintf( define_header, "\n"); }
	.  { fprintf( define_header, "%s", yytext); }
}

<ATTRIBUTE_DEFINE_CSS>{
	\n {
		if (isOldComment) {
			fprintf(define_header, "*/\n");
		} else {
			fprintf(define_header, "\n");
		}
		BEGIN(ATTRIBUTE_DEFINE);
	}
	.  { fprintf(define_header, "%s", yytext); }
}
<<EOF>> {
	if (isVerbose) printf("\nEND\n");
	return 0;
}
