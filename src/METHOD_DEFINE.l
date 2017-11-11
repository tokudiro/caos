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
        RBuf_enque(queue, "*", 1, T_POINTER);
    }

    \{ {
        RBuf_allDeque(queue, allque);
        /* source */
        fprintf( source, "%s{", allque );
        /* header */
            SLib_chopRet(allque);
        switch(methodVisibility){
        case VISIBILITY_PUBLIC:
            fprintf( public_header, "%s;\n", allque );
            break;
        case VISIBILITY_PRIVATE:
            if (!isNonPrivateHeader) {
                fprintf( private_header, "%s;\n", allque );
            }
            break;
        default:
            break;
        }
        arg_analysis = FALSE;
        if (isVerbose) printf("BEGIN <METHOD_SCOPE>\n");
        BEGIN(METHOD_SCOPE);
    }

    \( {
        boolean isMethod;
        if (arg_analysis)
        {
            if (arg_comma) {
                RBuf_enque(queue, ",", 1, T_COMMA);
                RBuf_enque(queue, " ", 1, T_WSPACE);
                arg_comma=FALSE;
            }
            RBuf_enque( queue, yytext, yyleng, T_NON);
        }
        else
        {
            isMethod = RBuf_back_retype(queue, T_WORD, T_METHOD);
            RBuf_trimQue(queue);
            RBuf_enque( queue, yytext, yyleng, T_NON);
            if (isMethod) {
                if (methodScope == SCOPE_OBJECT){
                    if(methodMutability == MUTABILITY_IMMUTABLE) {
                        RBuf_enque( queue, "const", 5, T_KEYWORD);
                        RBuf_enque( queue, " ", 1, T_WSPACE);
                    }
                    RBuf_enque( queue, SBuf_getStr(class_buf), SBuf_getLen(class_buf), T_CLASS);
                    RBuf_enque( queue, "*", 1, T_POINTER);
                    RBuf_enque( queue, " ", 1, T_WSPACE);
                    RBuf_enque( queue, SBuf_getStr(thispointer_buf), SBuf_getLen(thispointer_buf), T_VALUE);
                    arg_comma = TRUE;
                }
            }
            arg_analysis = TRUE;
        }
    }

    \) {
        arg_comma = FALSE;
        RBuf_enque( queue, yytext, yyleng, T_NON);
    }

    {OBJECT} { RBuf_enque(queue, &(yytext[1]), yyleng-1, T_OBJECT); }
    {TYPE}   {
        if (arg_comma) {
            RBuf_enque(queue, ",", 1, T_COMMA);
            RBuf_enque(queue, " ", 1, T_WSPACE);
            arg_comma=FALSE;
        }
        RBuf_enque(queue, yytext, yyleng, T_TYPE);
    }
    {VOID}   {
        if (arg_comma) {
            /* nothing */
        } else {
            RBuf_enque(queue, yytext, yyleng, T_KEYWORD);
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
