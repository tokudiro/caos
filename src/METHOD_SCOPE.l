<METHOD_SCOPE>{
	{NUMBER} {
	    RBuf_enque(tmp, yytext, yyleng, T_NUMBER);
		fprintf( source, "%s", RBuf_allque(tmp, allque, SBuf_getStr(class_buf)) );
	}

	{CLASS}  {
		fprintf(source, "%s", &(yytext[1]));
		SBuf_setBuf( obj_class_buf, &(yytext[1]), yyleng-1);
	}

	\{ {
	    RBuf_enque(tmp, yytext, yyleng, T_NON);
		fprintf( source, "%s", RBuf_allque(tmp, allque, SBuf_getStr(class_buf)) );
		bracket++;
	}

	\} {
	    RBuf_enque(tmp, yytext, yyleng, T_NON);
		fprintf( source, "%s", RBuf_allque(tmp, allque, SBuf_getStr(class_buf)) );
		if (bracket == 0) {
			methodVisibility = VISIBILITY_NONE;
    	    if (isVerbose) printf("BEGIN <CLASS_SCOPE>\n");
			BEGIN(CLASS_SCOPE);
		} else {
			bracket--;
		}
	}

	\( {
		boolean isMethod = RBuf_back_retype(tmp, T_OBJECT, T_METHOD);
		RBuf_enque( tmp, yytext, yyleng, T_NON);
		if (isMethod) {
			if (isThisPointer){
				RBuf_enque( tmp, "this", 4, T_KEYWORD);
			}else{
				RBuf_enque( tmp, "self", 4, T_KEYWORD);
			}
			arg_comma = TRUE;
		}
	}

	\) {
		arg_comma = FALSE;
	    RBuf_enque(tmp, yytext, yyleng, T_NON);
		fprintf( source, "%s", RBuf_allque(tmp, allque, SBuf_getStr(class_buf)) );
	}

	{OBJECT} {
		if (arg_comma) {
			RBuf_enque(tmp, ", ", 2, T_NON);
			arg_comma=FALSE;
		}
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
		if (arg_comma) {
			RBuf_enque(tmp, ", ", 2, T_NON);
			arg_comma=FALSE;
		}
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
	    RBuf_enque(tmp, yytext, yyleng, T_SEMICOLON);
		fprintf( source, "%s", RBuf_allque(tmp, allque, SBuf_getStr(class_buf)) );
	}
	[\+\-\=\*\/:,<>~&?|!%]  {
	    RBuf_enque(tmp, yytext, yyleng, T_NON);
		fprintf( source, "%s", RBuf_allque(tmp, allque, SBuf_getStr(class_buf)) );
	}
	.  { RBuf_enque(tmp, yytext, yyleng, T_NON); }
}

<METHOD_SCOPE_CSA>{
	{COMMENTE} { fprintf(source, "*/"); BEGIN(METHOD_SCOPE); }
	\n { fprintf( source, "\n"); }
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
	\n { fprintf( source, "\n"); }
	. { fprintf( source, "%s", yytext); }
}
