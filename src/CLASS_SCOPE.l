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
	    countPublicMethod++;
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
	    countPrivateMethod++;
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
	    countClassMethod++;
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
		fprintf( struct_header, "};\n" );
	    if (classType == TYPE_STRUCT) {
    	    fprintf( public_header, "#include \"%s\"\n", define_header_name );
	    }
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
		fprintf( (isMember?struct_header:source), "/*");
		BEGIN(CLASS_SCOPE_CSA);
	}
	{COMMENT}  {
		if (isOldComment) {
			fprintf( (isMember?struct_header:source), "/*");
		} else {
			fprintf( (isMember?struct_header:source), "//");
		}
		BEGIN(CLASS_SCOPE_CSS);
	}
    {DIRECTIVE_COMMENT} {
		BEGIN(CLASS_SCOPE_CS);
    }
	\n {
		fprintf( (isMember?struct_header:source), "%s", RBuf_allque(queue, allque, SBuf_getStr(class_buf)) );
		fprintf( (isMember?struct_header:source), "\n");
		if (isMember) { isMember = FALSE; }
	}
	;  {
		/* Class Member */
		RBuf_enque(queue, yytext, yyleng, T_SEMICOLON);
		fprintf(struct_header, "%s", RBuf_allque(queue, allque, SBuf_getStr(class_buf)) );
		isMember = TRUE;
	}
	
	=  {
		/* TODO : bad case : struct member initialize */
		if (isVerbose) printf("error:struct member initialize\n");
		/* Class Member */
		fprintf(struct_header, "%s;", RBuf_allque(queue, allque, SBuf_getStr(class_buf)) );
		isMember = TRUE;
	}
	.  { RBuf_enque(queue, yytext, yyleng, T_NON); }
}

<CLASS_SCOPE_CSA>{
	{COMMENTE} { fprintf( (isMember?struct_header:source), "*/"); BEGIN(CLASS_SCOPE); }
	\n { fprintf( (isMember?struct_header:source), "\n"); }
	.  { fprintf( (isMember?struct_header:source), "%s", yytext); }
}

<CLASS_SCOPE_CSS>{
	\n {
		if (isOldComment) {
			fprintf((isMember?struct_header:source), "*/\n");
		} else {
			fprintf((isMember?struct_header:source), "\n");
		}
		BEGIN(CLASS_SCOPE);
	}
	.  { fprintf((isMember?struct_header:source), "%s", yytext); }
}

<CLASS_SCOPE_CS>{
	\n {
		BEGIN(CLASS_SCOPE);
	}
    . {;}
}
