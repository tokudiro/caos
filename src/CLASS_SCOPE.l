<CLASS_SCOPE>{
	{NUMBER} {
		RBuf_enque(queue, yytext, yyleng, T_NUMBER);
	}

	{CLASS}  {
		RBuf_enque(queue, &(yytext[1]), yyleng-1, T_CLASS);
		if(isThisPointer) RBuf_enque(queue, "*", 1, T_KEYWORD);
	}

	{DIRECTIVE_PUBLIC_METHOD} {
		if (isVerbose) printf("#directive public method\n");
		if (isLineNo) fprintf(source, "#line %d \"%s\"\n", yylineno, input_filename);
		if (isLineNo) fprintf(public_header, "#line %d \"%s\"\n", yylineno, input_filename);
		methodVisibility = VISIBILITY_PUBLIC;
	    methodScope = SCOPE_OBJECT;
	    if (isVerbose) printf("BEGIN <METHOD_DEFINE>\n");
		BEGIN(METHOD_DEFINE);
	}
	{DIRECTIVE_PRIVATE_METHOD} {
		if (isVerbose) printf("#directive private method\n");
		if (isLineNo) fprintf(source, "#line %d \"%s\"\n", yylineno, input_filename);
		if (isLineNo) fprintf(private_header, "#line %d \"%s\"\n", yylineno, input_filename);
		fprintf(private_header, "static ");
		methodVisibility = VISIBILITY_PRIVATE;
	    methodScope = SCOPE_OBJECT;
	    if (isVerbose) printf("BEGIN <METHOD_DEFINE>\n");
		BEGIN(METHOD_DEFINE);
	}
	{DIRECTIVE_PROTECTED_METHOD} {
		//if (isLineNo) fprintf(source, "#line %d \"%s\"\n", yylineno, input_filename);
		//if (isLineNo) fprintf(protected_header, "#line %d \"%s\"\n", yylineno, input_filename);
		//methodVisibility = VISIBILITY_PROTECTED;
		//BEGIN(METHOD_DEFINE);
	}
	{DIRECTIVE_CLASS_METHOD} {
		if (isVerbose) printf("#directive class method\n");
		if (isLineNo) fprintf(source, "#line %d \"%s\"\n", yylineno, input_filename);
		if (isLineNo) fprintf(private_header, "#line %d \"%s\"\n", yylineno, input_filename);
		methodVisibility = VISIBILITY_PUBLIC;
	    methodScope = SCOPE_CLASS;
	    if (isVerbose) printf("BEGIN <METHOD_DEFINE>\n");
		BEGIN(METHOD_DEFINE);
	}
	
	{DIRECTIVE_PUBLIC_ATTRIBUTE} {
	    if (isVerbose) printf("BEGIN <ATTRIBUTE_DEFINE>\n");
		BEGIN(ATTRIBUTE_DEFINE);
	}
	
	\{ {
		char* method = RBuf_allque(queue, allque, SBuf_getStr(class_buf));
		if (strlen(method) != 0) {
			fprintf( public_header, "%s;\n", method );
		}
		fprintf( source, "%s{", method );
	    if (isVerbose) printf("BEGIN <METHOD_SCOPE>\n");
		BEGIN(METHOD_SCOPE);
	}

	{DIRECTIVE_ENDCLASS} {
		fprintf( define_header, "};\n" );
		SBuf_init(class_buf);
	    if (isVerbose) printf("BEGIN <INITIAL>\n");
		BEGIN(INITIAL);
	}

	\( {
		RBuf_enque( queue, yytext, yyleng, T_NON);
	}

	\) {
		arg_comma = FALSE;
		RBuf_enque( queue, ")", 1, T_NON);
	}

	{OBJECT} { RBuf_enque(queue, &(yytext[1]), yyleng-1, T_OBJECT); }
	{WORD}   {
		RBuf_enque(queue, yytext, yyleng, T_WORD);
	}
	{WSPACE}   { RBuf_enque(queue, yytext, yyleng, T_WSPACE); }
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
		fprintf( (isMember?define_header:source), "%s", RBuf_allque(queue, allque, SBuf_getStr(class_buf)) );
		fprintf( (isMember?define_header:source), "\n");
		if (isMember) { isMember = FALSE; }
	}
	;  {
		/* Class Member */
		RBuf_enque(queue, yytext, yyleng, T_SEMICOLON);
		fprintf(define_header, "%s", RBuf_allque(queue, allque, SBuf_getStr(class_buf)) );
		isMember = TRUE;
	}
	
	=  {
		/* TODO : bad case : struct member initialize */
		if (isVerbose) printf("error:struct member initialize\n");
		/* Class Member */
		fprintf(define_header, "%s;", RBuf_allque(queue, allque, SBuf_getStr(class_buf)) );
		isMember = TRUE;
	}
	.  { RBuf_enque(queue, yytext, yyleng, T_NON); }
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
