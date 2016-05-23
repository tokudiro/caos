<METHOD_SCOPE>{
	{NUMBER} {
	    RBuf_enque(queue, yytext, yyleng, T_NUMBER);
		fprintf( source, "%s", RBuf_allDeque(queue, allque, SBuf_getStr(class_buf)) );
	}

	{CLASS}  {
		fprintf(source, "%s", &(yytext[1]));
	}

	\{ {
	    RBuf_enque(queue, yytext, yyleng, T_NON);
		fprintf( source, "%s", RBuf_allDeque(queue, allque, SBuf_getStr(class_buf)) );
		bracket++;
	}

	\} {
	    RBuf_enque(queue, yytext, yyleng, T_NON);
		fprintf( source, "%s", RBuf_allDeque(queue, allque, SBuf_getStr(class_buf)) );
		if (bracket == 0) {
			methodVisibility = VISIBILITY_NONE;
    	    if (isVerbose) printf("BEGIN <CLASS_SCOPE>\n");
			BEGIN(CLASS_SCOPE);
		} else {
			bracket--;
		}
	}

	\( {
		boolean isMethod = RBuf_back_retype(queue, T_OBJECT, T_METHOD);
		RBuf_enque( queue, yytext, yyleng, T_NON);
		if (isMethod) {
			if (isThisPointer){
				RBuf_enque( queue, "this", 4, T_KEYWORD);
			}else{
				RBuf_enque( queue, "self", 4, T_KEYWORD);
			}
			arg_comma = TRUE;
		}
	}

	\) {
		arg_comma = FALSE;
	    RBuf_enque(queue, yytext, yyleng, T_NON);
		fprintf( source, "%s", RBuf_allDeque(queue, allque, SBuf_getStr(class_buf)) );
	}

	{OBJECT} {
		if (arg_comma) {
			RBuf_enque(queue, ", ", 2, T_NON);
			arg_comma=FALSE;
		}
		fprintf( source, "%s", RBuf_allDeque(queue, allque, SBuf_getStr(class_buf)) );
		RBuf_enque(queue, &(yytext[1]), yyleng-1, T_OBJECT);
	}
	{SELF_OBJECT} {
		fprintf( source, "%s", RBuf_allDeque(queue, allque, SBuf_getStr(class_buf)) );
		if (isThisPointer) {
			RBuf_enque(queue, "this", 4, T_KEYWORD);
		}else{
			RBuf_enque(queue, "self", 4, T_KEYWORD);
		}
	}
	{WORD}   {
		if (arg_comma) {
			RBuf_enque(queue, ",", 1, T_COMMA);
			RBuf_enque(queue, " ", 1, T_WSPACE);
			arg_comma=FALSE;
		}
		RBuf_enque(queue, yytext, yyleng, T_WORD);
	}
	{WSPACE} {
		if ( RBuf_isEmpty(queue) == TRUE ) {
			fprintf(source, "%s", yytext);
		} else {
			RBuf_enque(queue, yytext, yyleng, T_WSPACE);
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
    {DIRECTIVE_COMMENT} {
		BEGIN(METHOD_SCOPE_CS);
    }
	\"  { fprintf( source, "\""); BEGIN(METHOD_SCOPE_ST); }
	{DOT} {
		RBuf_enque(queue, yytext, yyleng, T_DOT);
	}
	\n { fprintf( source, "\n"); }
	;  {
	    RBuf_enque(queue, yytext, yyleng, T_SEMICOLON);
		fprintf( source, "%s", RBuf_allDeque(queue, allque, SBuf_getStr(class_buf)) );
	}
	[\+\-\=\*\/:,<>~&?|!%]  {
	    RBuf_enque(queue, yytext, yyleng, T_NON);
		fprintf( source, "%s", RBuf_allDeque(queue, allque, SBuf_getStr(class_buf)) );
	}
	.  { RBuf_enque(queue, yytext, yyleng, T_NON); }
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

<METHOD_SCOPE_CS>{
	\n {
		BEGIN(METHOD_SCOPE);
	}
    . {;}
}
