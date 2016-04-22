%{
	void class_define()
	{
		if (isVerbose) printf("class %s\n", SBuf_getStr(class_buf) );
		fprintf( source, "#include \"%s\"\n", define_header_name );
		fprintf( source, "#include \"%s\"\n", private_header_name );
		fprintf( source, "/* class %s */\n", SBuf_getStr(class_buf) );
		if (isLineNo) fprintf(source, "#line %d \"%s\"\n", yylineno, input_filename);

		fprintf( public_header, "/* class %s */\n", SBuf_getStr(class_buf) );
		if (isLineNo) fprintf(public_header, "#line %d \"%s\"\n", yylineno, input_filename);
		fprintf( public_header, "typedef struct %s_struct", SBuf_getStr(class_buf) );
		if (!isThisPointer) fprintf( public_header, "*");
		fprintf( public_header, " %s;\n", SBuf_getStr(class_buf) );

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
