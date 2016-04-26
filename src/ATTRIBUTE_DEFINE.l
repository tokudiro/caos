<ATTRIBUTE_DEFINE>{
	{NUMBER} {
		RBuf_enque(tmp, yytext, yyleng, T_NUMBER);
	}

	{CLASS}  {
		//if (arg_comma) { RBuf_enque(tmp, ", ", 2, T_NON); arg_comma=FALSE; }
		RBuf_enque(tmp, &(yytext[1]), yyleng-1, T_CLASS);
		if(isThisPointer) RBuf_enque(tmp, "*", 1, T_KEYWORD);
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
		fprintf(define_header, "%s;", RBuf_allque(tmp, allque, SBuf_getStr(class_buf)) );
		isMember = TRUE;
	}
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
