<METHOD_DEFINE>{
	{NUMBER} {
		RBuf_enque(tmp, yytext, yyleng, T_NUMBER);
	}

	{CLASS}  {
		if (arg_comma) {
			RBuf_enque(tmp, ",", 1, T_COMMA);
			RBuf_enque(tmp, " ", 1, T_WSPACE);
			arg_comma=FALSE;
		}
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
	    if (isVerbose) printf("BEGIN <METHOD_SCOPE>\n");
		BEGIN(METHOD_SCOPE);
	}

	\( {
		boolean isMethod = RBuf_back_retype(tmp, T_WORD, T_METHOD);
		RBuf_enque( tmp, yytext, yyleng, T_NON);
		if (isMethod) {
			RBuf_enque( tmp, SBuf_getStr(class_buf), SBuf_getLen(class_buf), T_CLASS);
			if (isThisPointer){
				RBuf_enque( tmp, "*", 1, T_ASTERISK);
			    RBuf_enque( tmp, " ", 1, T_NON);
				RBuf_enque( tmp, "this", 4, T_KEYWORD);
			}else{
			    RBuf_enque( tmp, " ", 1, T_NON);
				RBuf_enque( tmp, "self", 4, T_KEYWORD);
			}
			arg_comma = TRUE;
		}
	}

	\) {
		arg_comma = FALSE;
		RBuf_enque( tmp, yytext, yyleng, T_NON);
	}

	{OBJECT} { RBuf_enque(tmp, &(yytext[1]), yyleng-1, T_OBJECT); }
	{WORD}   {
		if ( (arg_comma) && ( 0 == strcmp(yytext, "void") ) ) {
			/* nothing */
		} else {
			if (arg_comma) {
				RBuf_enque(tmp, ", ", 2, T_NON);
				arg_comma=FALSE;
			}
			RBuf_enque(tmp, yytext, yyleng, T_WORD);
		}
	}
	{WSPACE}   { RBuf_enque(tmp, yytext, yyleng, T_WSPACE); }
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
