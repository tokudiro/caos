%{
	void defineClassInSource()
	{
		if (isVerbose) printf("class %s\n", SBuf_getStr(class_buf) );
		fprintf( source, "#include \"%s\"\n", define_header_name );
		fprintf( source, "#include \"%s\"\n", private_header_name );
		fprintf( source, "/**\n");
		fprintf( source, "* \\addtogroup %s\n", SBuf_getStr(class_buf));
		fprintf( source, "* \\{\n" );
		fprintf( source, "*/\n" );
		if (singleton) {
			fprintf( source, "static struct %s_struct instance;\n", SBuf_getStr(class_buf) );
			fprintf( source, "%s", SBuf_getStr(class_buf) );
			if (isThisPointer) fprintf( source, "*");
			fprintf( source, " %s_getInstance() { return &instance; }\n", SBuf_getStr(class_buf) );
		}
		if (isLineNo) fprintf(source, "#line %d \"%s\"\n", yylineno, input_filename);
	}

	void defineClassInHeader()
	{
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
		fprintf( define_header, "/**\n");
		fprintf( define_header, "* \\defgroup %s\n", SBuf_getStr(class_buf));
		fprintf( define_header, "* \\{\n" );
		fprintf( define_header, "*/\n" );
		if (isLineNo) fprintf(define_header, "#line %d \"%s\"\n", yylineno, input_filename);
		fprintf( define_header, "struct %s_struct{\n", SBuf_getStr(class_buf) );
	}
%}

<CLASS_DEFINE>{
	{WORD}  {
		SBuf_setBuf( class_buf, yytext, yyleng);
		defineClassInSource();
		defineClassInHeader();
		BEGIN(CLASS_SCOPE);
	}
	
	{CLASS}  {
		SBuf_setBuf( class_buf, &(yytext[1]), yyleng-1);
		defineClassInSource();
		defineClassInHeader();
		BEGIN(CLASS_SCOPE);
	}

	\n { fprintf( source, "\n" ); }
}
