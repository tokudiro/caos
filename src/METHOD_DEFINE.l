<METHOD_DEFINE>{
	{NUMBER} {
		RBuf_enque(queue, yytext, yyleng, T_NUMBER);
	}

	{CLASS}  {
		if (arg_comma) {
			RBuf_enque(queue, ",", 1, T_COMMA);
			RBuf_enque(queue, " ", 1, T_WSPACE);
			arg_comma=FALSE;
		}
		RBuf_enque(queue, &(yytext[1]), yyleng-1, T_CLASS);
		if (isThisPointer) RBuf_enque(queue, "*", 1, T_KEYWORD);
	}

	\{ {
		RBuf_allque(queue, allque, SBuf_getStr(class_buf));
        // source
	    fprintf( source, "%s{", allque );
	    // header
	        SLib_chopRet(allque);
		switch(methodVisibility){
		case VISIBILITY_PUBLIC:
			fprintf( public_header, "%s;\n", allque );
			break;
		case VISIBILITY_PRIVATE:
			fprintf( private_header, "%s;\n", allque );
			break;
		default:
			break;
		}
	    if (isVerbose) printf("BEGIN <METHOD_SCOPE>\n");
		BEGIN(METHOD_SCOPE);
	}

	\( {
		boolean isMethod = RBuf_back_retype(queue, T_WORD, T_METHOD);
	    RBuf_trimque(queue);
		RBuf_enque( queue, yytext, yyleng, T_NON);
		if (isMethod) {
		    if (methodScope == SCOPE_OBJECT){
    			RBuf_enque( queue, SBuf_getStr(class_buf), SBuf_getLen(class_buf), T_CLASS);
    			if (isThisPointer){
    				RBuf_enque( queue, "*", 1, T_ASTERISK);
    			    RBuf_enque( queue, " ", 1, T_WSPACE);
    				RBuf_enque( queue, "this", 4, T_KEYWORD);
    			}else{
    			    RBuf_enque( queue, " ", 1, T_WSPACE);
    				RBuf_enque( queue, "self", 4, T_KEYWORD);
    			}
    			arg_comma = TRUE;
		    }
		}
	}

	\) {
		arg_comma = FALSE;
		RBuf_enque( queue, yytext, yyleng, T_NON);
	}

	{OBJECT} { RBuf_enque(queue, &(yytext[1]), yyleng-1, T_OBJECT); }
	{WORD}   {
		if ( (arg_comma) && ( 0 == strcmp(yytext, "void") ) ) {
			/* nothing */
		} else {
			if (arg_comma) {
				RBuf_enque(queue, ", ", 2, T_NON);
				arg_comma=FALSE;
			}
			RBuf_enque(queue, yytext, yyleng, T_WORD);
		}
	}
	{WSPACE}   { RBuf_enque(queue, yytext, yyleng, T_WSPACE); }
	{COMMENTS} { fprintf( source, "/*"); BEGIN(METHOD_DEFINE_CSA); }
	{COMMENT}  {
		if (isOldComment) {
			fprintf( source, "/*");
		} else {
			fprintf( source, "//");
		}
		BEGIN(METHOD_DEFINE_CSS);
	}
	\n { RBuf_enque(queue, yytext, yyleng, T_NON); }
	.  { RBuf_enque(queue, yytext, yyleng, T_NON); }
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
