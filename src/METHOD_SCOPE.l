%{
    void check_arg_comma() {
        if (arg_comma) {
            RBuf_enque(queue, ",", 1, T_COMMA);
            RBuf_enque(queue, " ", 1, T_WSPACE);
            arg_comma=FALSE;
        }
    }
%}

<METHOD_SCOPE>{
	{NUMBER} {
	    check_arg_comma();
	    RBuf_enque(queue, yytext, yyleng, T_NUMBER);
		fprintf( source, "%s", RBuf_allDeque(queue, allque) );
	}

	{CLASS}  {
	    int length = SLib_checkLength( &(yytext[1]) );
	    RBuf_enque(queue, &(yytext[1]), length, T_WORD);
	    RBuf_enque(queue, "*", 1, T_ASTERISK);
        fprintf( source, "%s", RBuf_allDeque(queue, allque) );
	}

	\{ {
	    RBuf_enque(queue, yytext, yyleng, T_NON);
		fprintf( source, "%s", RBuf_allDeque(queue, allque) );
		bracket++;
	}

	\} {
	    RBuf_enque(queue, yytext, yyleng, T_NON);
		fprintf( source, "%s", RBuf_allDeque(queue, allque) );
		if (bracket == 0) {
			methodVisibility = VISIBILITY_NONE;
    	    if (isVerbose) printf("BEGIN <CLASS_SCOPE>\n");
			BEGIN(CLASS_SCOPE);
		} else {
			bracket--;
		}
	}

	\( {
	    check_arg_comma();
		boolean isMethod = RBuf_back_retype(queue, T_OBJECT, T_METHOD);
		RBuf_enque( queue, yytext, yyleng, T_NON);
		if (isMethod) {
			RBuf_enque( queue, SBuf_getStr(thispointer_buf), SBuf_getLen(thispointer_buf), T_KEYWORD);
			arg_comma = TRUE;
		}
	}

	\) {
		arg_comma = FALSE;
	    RBuf_enque(queue, yytext, yyleng, T_NON);
		fprintf( source, "%s", RBuf_allDeque(queue, allque) );
	}

	{OBJECT} {
	    check_arg_comma();
		fprintf( source, "%s", RBuf_allDeque(queue, allque) );
		RBuf_enque(queue, &(yytext[1]), yyleng-1, T_OBJECT);
	}
	{SELF_OBJECT} {
		fprintf( source, "%s", RBuf_allDeque(queue, allque) );
		RBuf_enque(queue, SBuf_getStr(thispointer_buf), SBuf_getLen(thispointer_buf), T_KEYWORD);
	}
	{WORD}   {
	    check_arg_comma();
		RBuf_enque(queue, yytext, yyleng, T_WORD);
	}
	{WSPACE} {
		if ( RBuf_isEmpty(queue) == TRUE ) {
			fprintf(source, "%s", yytext);
		} else {
			RBuf_enque(queue, yytext, yyleng, T_WSPACE);
		}
	}
	{COMMENTS} {
        RBuf_enque(queue, yytext, yyleng, T_NON);
	    BEGIN(METHOD_SCOPE_CSA);
	}
	{COMMENT}  {
		if (isOldComment) {
            RBuf_enque(queue, "/*", 2, T_NON);
			BEGIN(METHOD_SCOPE_CSS);
		} else {
            RBuf_enque(queue, yytext, yyleng, T_NON);
            BEGIN(METHOD_SCOPE_CSS);
		}
	}
    {DIRECTIVE_COMMENT} {
		BEGIN(METHOD_SCOPE_CS);
    }
	\"  { RBuf_enque(queue, yytext, yyleng, T_NON); BEGIN(METHOD_SCOPE_ST); }
	{DOT} {
		RBuf_enque(queue, yytext, yyleng, T_DOT);
	}
    \n { RBuf_enque(queue, yytext, yyleng, T_NON); }
	;  {
	    RBuf_enque(queue, yytext, yyleng, T_SEMICOLON);
		fprintf( source, "%s", RBuf_allDeque(queue, allque) );
	}
	[\+\-\=\*\/:,<>~&?|!%]  {
	    check_arg_comma();
	    RBuf_enque(queue, yytext, yyleng, T_NON);
		fprintf( source, "%s", RBuf_allDeque(queue, allque) );
	}
	.  { RBuf_enque(queue, yytext, yyleng, T_NON); }
}

<METHOD_SCOPE_CSA>{
    {COMMENTE} {
        RBuf_enque(queue, yytext, yyleng, T_NON);
        BEGIN(METHOD_SCOPE);
    }
    {WORD} { RBuf_enque(queue, yytext, yyleng, T_NON); }
    \n { RBuf_enque(queue, yytext, yyleng, T_NON); }
	.  { RBuf_enque(queue, yytext, yyleng, T_NON); }
}

<METHOD_SCOPE_CSS>{
    {WORD} { RBuf_enque(queue, yytext, yyleng, T_NON); }
	\n {
		if (isOldComment) {
            RBuf_enque(queue, "*/", 2, T_NON);
            RBuf_enque(queue, yytext, yyleng, T_NON);
		    BEGIN(METHOD_SCOPE);
		} else {
            RBuf_enque(queue, yytext, yyleng, T_NON);
            BEGIN(METHOD_SCOPE);
		}
	}
	.  { RBuf_enque(queue, yytext, yyleng, T_NON); }
}

<METHOD_SCOPE_ST>{
    \\\" { RBuf_enque(queue, yytext, yyleng, T_NON); }
    {WORD} { RBuf_enque(queue, yytext, yyleng, T_NON); }
    \"   { RBuf_enque(queue, yytext, yyleng, T_NON); BEGIN(METHOD_SCOPE); }
    \n   { RBuf_enque(queue, yytext, yyleng, T_NON); }
    . { RBuf_enque(queue, yytext, yyleng, T_NON); }
}

<METHOD_SCOPE_CS>{
	\n {
		BEGIN(METHOD_SCOPE);
	}
    . {;}
}
