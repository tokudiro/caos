%{
	void getterInHeader(const char* type, const char* member)
	{
		fprintf(public_header, "%s %s_get_%s(", type, SBuf_getStr(class_buf), member);
		fprintf(public_header, "const %s* %s", SBuf_getStr(class_buf), SBuf_getStr(thispointer_buf) );
		fprintf(public_header, ");\n");
	}

	void getterInSource(const char* type, const char* member)
	{
		fprintf(source, "%s %s_get_%s(", type, SBuf_getStr(class_buf), member);
		fprintf(source, "const %s* %s", SBuf_getStr(class_buf), SBuf_getStr(thispointer_buf) );
		fprintf(source, ") { return ");
		fprintf(source, "%s->%s", SBuf_getStr(thispointer_buf), member );
		fprintf(source, "; }\n");
	}

	void setterInHeader(const char* type, const char* member)
	{
		fprintf(public_header, "void %s_set_%s(", SBuf_getStr(class_buf), member);
		fprintf(public_header, "%s* %s", SBuf_getStr(class_buf), SBuf_getStr(thispointer_buf) );
		fprintf(public_header, ", %s %s);\n", type, member);
	}

	void setterInSource(const char* type, const char* member)
	{
		fprintf(source, "void %s_set_%s(", SBuf_getStr(class_buf), member);
		fprintf(source, "%s* %s", SBuf_getStr(class_buf), SBuf_getStr(thispointer_buf) );
		fprintf(source, ", %s %s) { ", type, member);
		fprintf(source, "%s->%s = %s", SBuf_getStr(thispointer_buf), member, member );
		fprintf(source, "; }\n");
	}

	/* a const value (not a pointer to const) cannot be assigned */
	boolean isAssignableType(const char* type)
	{
		return (strstr(type, "const") == 0 || strchr(type, '*') != 0) ? TRUE : FALSE;
	}
%}

<ATTRIBUTE_DEFINE>{
	{NUMBER} {
		RBuf_enque(queue, yytext, yyleng, T_NUMBER);
	}

	{CLASS}  {
		RBuf_enque(queue, &(yytext[1]), yyleng-1, T_CLASS);
		RBuf_enque(queue, "*", 1, T_ASTERISK);
	}

	{DIRECTIVE_ENDCLASS} {
        if (!isNonDefineHeader) {
    		fprintf( struct_header, "};\n" );
	    }
		finishClass();
		SBuf_init(class_buf);
        if (isVerbose) printf("BEGIN <INITIAL>\n");
		BEGIN(INITIAL);
	}

	{OBJECT} { RBuf_enque(queue, &(yytext[1]), yyleng-1, T_OBJECT); }
	{WORD}   {
		RBuf_enque(queue, yytext, yyleng, T_WORD);
	}
	{WSPACE}   { RBuf_enque(queue, yytext, yyleng, T_WSPACE); }
	{COMMENTS} {
        isCommentQueued = isDeclarationQueued(queue);
        if (isCommentQueued) {
            queued_comment_put("/*");
        } else {
            if (RBuf_isWSPACE(queue)) {
                RBuf_allDeque( queue, allque);
                if (isMember){
                    if (!isNonDefineHeader) {
                        fprintf( struct_header, "%s", allque);
                    }
                }else{
                    fprintf( source, "%s", allque);
                }
            }
            if (!isNonDefineHeader) {
                fprintf( struct_header, "/*");
            }
        }
        BEGIN(ATTRIBUTE_DEFINE_CSA);
	}
	{COMMENT}  {
        isCommentQueued = isDeclarationQueued(queue);
        if (isCommentQueued) {
            queued_comment_put(isOldComment ? "/*" : "//");
        } else {
            if (RBuf_isWSPACE(queue)) {
                RBuf_allDeque( queue, allque);
                if (isMember){
                    if (!isNonDefineHeader) {
                        fprintf( struct_header, "%s", allque);
                    }
                }else{
                    fprintf( source, "%s", allque);
                }
            }
            if (!isNonDefineHeader) {
                fprintf( struct_header, isOldComment ? "/*" : "//");
            }
        }
		BEGIN(ATTRIBUTE_DEFINE_CSS);
	}
	\n {
        if (!isNonDefineHeader) {
        	fprintf( struct_header, "%s", RBuf_allDeque(queue, allque) );
        	fprintf( struct_header, "\n");
        }
		if (isMember) { isMember = FALSE; }
        if (isVerbose) printf("BEGIN <CLASS_SCOPE>\n");
		BEGIN(CLASS_SCOPE);
	}
	;  {
        char type[MAX_TEXT];
	    RBuf_enque(queue, yytext, yyleng, T_SEMICOLON);
	    RBuf_trimQue(queue);
		/* Class Member */
	    countAttribute++;
	    
		isMember = RBuf_back_retype(queue, T_WORD, T_PARAMETER);
		if (isMember && RBuf_typeBeforeParameter(queue, type) != 0 && type[0] != 0) {
			const char* member = RBuf_back_getStr(queue, T_PARAMETER);
			
			/* getter,setter in header */
			getterInHeader(type, member);
			if (isAssignableType(type)) setterInHeader(type, member);
			/* getter,setter in source */
			getterInSource(type, member);
			if (isAssignableType(type)) setterInSource(type, member);
		}
        RBuf_allDeque(queue, allque);
        Vars_declareMember(allque);
        if (!isNonDefineHeader) {
    		fprintf(struct_header, "%s", allque );
        }
	}
	=  {
		/* a struct member cannot have an initializer : skip it up to ';' */
		reportError("member initializer is not supported (initialize it in a method)");
		BEGIN(ATTRIBUTE_DEFINE_INIT);
	}
	\*  { RBuf_enque(queue, yytext, yyleng, T_ASTERISK); }
	.  { RBuf_enque(queue, yytext, yyleng, T_NON); }
}

<ATTRIBUTE_DEFINE_CSA>{
	{COMMENTE} {
        if (isCommentQueued) {
            queued_comment_put("*/");
            queued_comment_flush();
            isCommentQueued = FALSE;
        } else if (!isNonDefineHeader) {
    		fprintf( struct_header, "*/");
        }
		BEGIN(ATTRIBUTE_DEFINE);
	}
	\n {
        if (isCommentQueued) {
            queued_comment_put("\n");
        } else if (!isNonDefineHeader) {
            fprintf( struct_header, "\n");
        }
	}
	.  {
        if (isCommentQueued) {
            queued_comment_put(yytext);
        } else if (!isNonDefineHeader) {
    	    fprintf( struct_header, "%s", yytext);
        }
	}
}

<ATTRIBUTE_DEFINE_CSS>{
	\n {
        const char* end = isOldComment ? "*/\n" : "\n";
        if (isCommentQueued) {
            queued_comment_put(end);
            queued_comment_flush();
            isCommentQueued = FALSE;
        } else if (!isNonDefineHeader) {
    		fprintf(struct_header, "%s", end);
        }
		BEGIN(ATTRIBUTE_DEFINE);
	}
	.  {
        if (isCommentQueued) {
            queued_comment_put(yytext);
        } else if (!isNonDefineHeader) {
    	    fprintf(struct_header, "%s", yytext);
        }
	}
}

<ATTRIBUTE_DEFINE_INIT>{
	\"([^"\\\n]|\\.)*\" {;}
	;  {
		yyless(0);
		BEGIN(ATTRIBUTE_DEFINE);
	}
	.|\n {;}
}
