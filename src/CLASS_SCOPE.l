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
	    methodMutability = MUTABILITY_MUTABLE;
	    methodScope = SCOPE_OBJECT;
	    if (isVerbose) printf("BEGIN <METHOD_DEFINE>\n");
		BEGIN(METHOD_DEFINE);
	}
	{DIRECTIVE_PRIVATE_METHOD} {
		if (isVerbose) printf("#directive private method\n");
		if (isLineNo) fprintf(source, "#line %d \"%s\"\n", yylineno, input_filename);
        if (!isNonPrivateHeader) {
    		if (isLineNo) fprintf(private_header, "#line %d \"%s\"\n", yylineno, input_filename);
    		fprintf(private_header, "static ");
        }
        countPrivateMethod++;
		methodVisibility = VISIBILITY_PRIVATE;
	    methodMutability = MUTABILITY_MUTABLE;
	    methodScope = SCOPE_OBJECT;
	    if (isVerbose) printf("BEGIN <METHOD_DEFINE>\n");
		BEGIN(METHOD_DEFINE);
	}
	{DIRECTIVE_PUBLIC_CONST_METHOD} {
		if (isVerbose) printf("#directive public const method\n");
		if (isLineNo) fprintf(source, "#line %d \"%s\"\n", yylineno, input_filename);
		if (isLineNo) fprintf(public_header, "#line %d \"%s\"\n", yylineno, input_filename);
	    countPublicMethod++;
		methodVisibility = VISIBILITY_PUBLIC;
	    methodMutability = MUTABILITY_IMMUTABLE;
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
        if (!isNonPrivateHeader) {
            if (isLineNo) fprintf(private_header, "#line %d \"%s\"\n", yylineno, input_filename);
        }
	    countClassMethod++;
		methodVisibility = VISIBILITY_PUBLIC;
	    methodMutability = MUTABILITY_MUTABLE;
	    methodScope = SCOPE_CLASS;
	    if (isVerbose) printf("BEGIN <METHOD_DEFINE>\n");
		BEGIN(METHOD_DEFINE);
	}
	
	{DIRECTIVE_PUBLIC_ATTRIBUTE} {
	    if (isVerbose) printf("BEGIN <ATTRIBUTE_DEFINE>\n");
		BEGIN(ATTRIBUTE_DEFINE);
	}
	
	\{ {
		char* method = RBuf_allDeque(queue, allque);
		if (strlen(method) != 0) {
			fprintf( public_header, "%s;\n", method );
		}
		fprintf( source, "%s{", method );
	    if (isVerbose) printf("BEGIN <METHOD_SCOPE>\n");
		BEGIN(METHOD_SCOPE);
	}

	{DIRECTIVE_ENDCLASS} {
	    if (!isNonDefineHeader) {
    		fprintf( struct_header, "};\n" );
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
        if (RBuf_isWSPACE)
        {
            RBuf_allDeque( queue, allque);
            if (isMember) {
                if (!isNonDefineHeader) {
                    fprintf( struct_header, "%s", allque);
                }
            }else{
                fprintf( source, "%s", allque);
            }
        }
        if (isMember) {
            if (!isNonDefineHeader) {
                fprintf( struct_header, "/*");
            }
        } else {
            fprintf( source, "/*");
        }
        BEGIN(CLASS_SCOPE_CSA);
	}
	{COMMENT}  {
        if (RBuf_isWSPACE)
        {
            RBuf_allDeque( queue, allque);
            if (isMember) {
                if (!isNonDefineHeader) {
                    fprintf( struct_header, "%s", allque);
                }
            }else{
                fprintf( source, "%s", allque);
            }
        }
		if (isOldComment) {
            if (isMember) {
                if (!isNonDefineHeader) {
        			fprintf( struct_header, "/*");
                }
            }else{
        		fprintf( source, "/*");
            }
		} else {
            if (isMember) {
                if (!isNonDefineHeader) {
        			fprintf( struct_header, "//");
                }
            }else{
    			fprintf( source, "//");
            }
		}
		BEGIN(CLASS_SCOPE_CSS);
	}
    {DIRECTIVE_COMMENT} {
		BEGIN(CLASS_SCOPE_CS);
    }
	\n {
	    RBuf_allDeque( queue, allque);
        if (isMember) {
            if (!isNonDefineHeader) {
        		fprintf( struct_header, "%s", allque);
        		fprintf( struct_header, "\n");
            }
    		isMember = FALSE;
        }else{
    		fprintf( source, "%s", allque);
    		fprintf( source, "\n");
        }
	}
	;  {
		/* Class Member */
		RBuf_enque(queue, yytext, yyleng, T_SEMICOLON);
	    RBuf_allDeque(queue, allque);
        if (!isNonDefineHeader) {
    		fprintf(struct_header, "%s", allque );
        }
		isMember = TRUE;
	}
	
	=  {
		/* TODO : bad case : struct member initialize */
		if (isVerbose) printf("error:struct member initialize\n");
		/* Class Member */
	    RBuf_allDeque( queue, allque);
        if (!isNonDefineHeader) {
    		fprintf(struct_header, "%s;", allque );
        }
		isMember = TRUE;
	}
	.  { RBuf_enque(queue, yytext, yyleng, T_NON); }
}

<CLASS_SCOPE_CSA>{
	{COMMENTE} {
        if (isMember) {
            if (!isNonDefineHeader) {
        	    fprintf( struct_header, "*/");
            }
        }else{
            fprintf( source, "*/");
        }
        BEGIN(CLASS_SCOPE);
    }
	\n {
        if (isMember) {
            if (!isNonDefineHeader) {
                fprintf( struct_header, "\n");
            }
        }else{
            fprintf( source, "\n");
        }
	}
	.  {
        if (isMember) {
            if (!isNonDefineHeader) {
                fprintf( struct_header, "%s", yytext);
            }
        }else{
            fprintf( source, "%s", yytext);
        }
	}
}

<CLASS_SCOPE_CSS>{
	\n {
		if (isOldComment) {
            if (isMember) {
                if (!isNonDefineHeader) {
                    fprintf(struct_header, "*/\n");
                }
            }else{
                fprintf(source, "*/\n");
            }
		} else {
            if (isMember) {
                if (!isNonDefineHeader) {
                    fprintf(struct_header, "\n");
                }
            }else{
                fprintf(source, "\n");
            }
		}
		BEGIN(CLASS_SCOPE);
	}
	.  {
        if (isMember) {
            if (!isNonDefineHeader) {
                fprintf(struct_header, "%s", yytext);
            }
        }else{
            fprintf(source, "%s", yytext);
        }
	}
}

<CLASS_SCOPE_CS>{
	\n {
		BEGIN(CLASS_SCOPE);
	}
    . {;}
}
