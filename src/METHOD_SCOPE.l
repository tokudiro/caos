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
    {DIRECTIVE_INVARIANT}.* |
    {DIRECTIVE_NOINVARIANT}.* {
        reportError("#invariant belongs in a class, before its methods; #noinvariant belongs before the '{'");
    }
    {DIRECTIVE_REQUIRE}.* |
    {DIRECTIVE_ENSURE}.*  {
        reportError("#require and #ensure belong between the header of a method and its '{'");
    }

    {NUMBER} {
        check_arg_comma();
        RBuf_enque(queue, yytext, yyleng, T_NUMBER);
        fprintf( source, "%s", RBuf_allDeque(queue, allque) );
    }

    {CLASS}  {
        int length = SLib_checkLength( &(yytext[1]) );
        RBuf_enque(queue, &(yytext[1]), length, T_CLASS);
        RBuf_enque(queue, "*", 1, T_POINTER);
        //fprintf( source, "%s", RBuf_allDeque(queue, allque) );
    }

    \{ {
        RBuf_enque(queue, yytext, yyleng, T_NON);
        fprintf( source, "%s", RBuf_allDeque(queue, allque) );
        bracket++;
    }

    \} {
        if (bracket == 0 && contractMethod.hasEnsure && contractMethod.isVoid && !contractMethod.endDone && injectDepth == 0) {
            /* the postconditions of a void method that ends without return */
            contractMethod.endDone = TRUE;
            yyless(0);
            PUSH_INPUT(contractMethod.endText, yylineno, input_filename, YY_START, TRUE);
        } else {
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
    }

    \( {
        check_arg_comma();
        boolean isMethod = RBuf_back_retype_stop(queue, T_OBJECT, T_METHOD, T_ASTERISK);
        RBuf_enque( queue, yytext, yyleng, T_NON);
        if (isMethod) {
            RBuf_enque( queue, SBuf_getStr(thispointer_buf), SBuf_getLen(thispointer_buf), T_VALUE);
            arg_comma = TRUE;
        }
    }

    \) {
        arg_comma = FALSE;
        RBuf_enque(queue, yytext, yyleng, T_NON);
        fprintf( source, "%s", RBuf_allDeque(queue, allque) );
    }

    {WORD}[[:blank:]]*("->"|"."){WORD}[[:blank:]]*"(" {
        /* var->method( : a method of the class of the variable */
        char call[1024];
        size_t keep = 0;
        if (resolveCall(yytext, FALSE, call, sizeof(call), &keep)) {
            check_arg_comma();
            RBuf_enque(queue, call, strlen(call), T_NON);
            arg_comma = TRUE;
        } else {
            yyless(keep);
            check_arg_comma();
            RBuf_enque(queue, yytext, yyleng, T_WORD);
        }
    }

    {OBJECT}[[:blank:]]*("->"|"."){WORD}[[:blank:]]*"(" {
        /* @member->method( : a method of the class of the member */
        char call[1024];
        size_t keep = 0;
        if (resolveCall(yytext, TRUE, call, sizeof(call), &keep)) {
            check_arg_comma();
            RBuf_enque(queue, call, strlen(call), T_NON);
            arg_comma = TRUE;
        } else {
            yyless(keep);
            check_arg_comma();
            fprintf( source, "%s", RBuf_allDeque(queue, allque) );
            RBuf_enque(queue, &(yytext[1]), yyleng-1, T_OBJECT);
        }
    }

    {OBJECT} {
        check_arg_comma();
        fprintf( source, "%s", RBuf_allDeque(queue, allque) );
        RBuf_enque(queue, &(yytext[1]), yyleng-1, T_OBJECT);
    }
    {SELF_OBJECT} {
        fprintf( source, "%s", RBuf_allDeque(queue, allque) );
        RBuf_enque(queue, SBuf_getStr(thispointer_buf), SBuf_getLen(thispointer_buf), T_VALUE);
    }
    {TYPE}   {
        check_arg_comma();
        RBuf_enque(queue, yytext, yyleng, T_TYPE);
    }
    {KEYWORD} {
        check_arg_comma();
        if (contractMethod.hasEnsure && injectDepth == 0) {
            /* return x;  ->  { result = x; <postconditions> return result; } */
            bracket++;
            contractMethod.inReturn = TRUE;
            RBuf_enque(queue, contractMethod.isVoid ? "{" : "{ result =", contractMethod.isVoid ? 1 : 10, T_NON);
        } else {
            RBuf_enque(queue, yytext, yyleng, T_KEYWORD);
        }
    }
    \* {
        check_arg_comma();
        if ( RBuf_back_getStr(queue, T_TYPE) || RBuf_back_getStr(queue, T_CLASS) )
        {
            RBuf_enque(queue, "*", 1, T_POINTER);
        }
        else
        {
            RBuf_enque(queue, "*", 1, T_ASTERISK);
        }
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
    \"  { check_arg_comma(); RBuf_enque(queue, yytext, yyleng, T_NON); BEGIN(METHOD_SCOPE_ST); }
    {DOT} {
        RBuf_enque(queue, yytext, yyleng, T_DOT);
    }
    \n { RBuf_enque(queue, yytext, yyleng, T_NON); }
    ;  {
        if (contractMethod.inReturn) {
            contractMethod.inReturn = FALSE;
            if (!contractMethod.isVoid) RBuf_enque(queue, yytext, yyleng, T_SEMICOLON);
            fprintf( source, "%s", RBuf_allDeque(queue, allque) );
            PUSH_INPUT(contractMethod.returnText, yylineno, input_filename, YY_START, TRUE);
        } else {
            RBuf_enque(queue, yytext, yyleng, T_SEMICOLON);
            RBuf_allDeque(queue, allque);
            Vars_declareText(allque);
            fprintf( source, "%s", allque );
        }
    }
    [\+\-\=\*\/:,<>~&?|!%]  {
        check_arg_comma();
        RBuf_enque(queue, yytext, yyleng, T_NON);
        RBuf_allDeque(queue, allque);
        if (yytext[0] == '=') Vars_declareText(allque);
        fprintf( source, "%s", allque );
    }
    .  {
        if (yytext[0] == '\'') check_arg_comma();
        RBuf_enque(queue, yytext, yyleng, T_NON);
    }
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
