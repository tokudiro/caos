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
	{COMMENT}  { fprintf( source, "/*"); BEGIN(METHOD_DEFINE_CSS); }
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
	\n { fprintf( source, "*/\n"); BEGIN(METHOD_DEFINE); }
	.  { fprintf( source, "%s", yytext); }
}
