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
		if (isVerbose) printf("#directive public method\n");
		if (isLineNo) fprintf(source, "#line %d \"%s\"\n", yylineno, input_filename);
		if (isLineNo) fprintf(public_header, "#line %d \"%s\"\n", yylineno, input_filename);
		methodVisibility = VISIBILITY_PUBLIC;
	    if (isVerbose) printf("BEGIN <METHOD_DEFINE>\n");
		BEGIN(METHOD_DEFINE);
	}
	{DIRECTIVE_PRIVATE_METHOD} {
		if (isVerbose) printf("#directive private method\n");
		if (isLineNo) fprintf(source, "#line %d \"%s\"\n", yylineno, input_filename);
		if (isLineNo) fprintf(private_header, "#line %d \"%s\"\n", yylineno, input_filename);
		fprintf(private_header, "static ");
		methodVisibility = VISIBILITY_PRIVATE;
	    if (isVerbose) printf("BEGIN <METHOD_DEFINE>\n");
		BEGIN(METHOD_DEFINE);
	}
	{DIRECTIVE_PROTECTED_METHOD} {
		//if (isLineNo) fprintf(source, "#line %d \"%s\"\n", yylineno, input_filename);
		//if (isLineNo) fprintf(protected_header, "#line %d \"%s\"\n", yylineno, input_filename);
		//methodVisibility = VISIBILITY_PROTECTED;
		//BEGIN(METHOD_DEFINE);
	}
	
	{DIRECTIVE_PUBLIC_ATTRIBUTE} {
	    if (isVerbose) printf("BEGIN <ATTRIBUTE_DEFINE>\n");
		BEGIN(ATTRIBUTE_DEFINE);
	}
	
	\{ {
		char* method = RBuf_allque(tmp, allque, SBuf_getStr(class_buf));
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
		fprintf( source, "/** \\} */\n" );
		fprintf( define_header, "/** \\} */\n" );
	    if (isVerbose) printf("BEGIN <INITIAL>\n");
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
