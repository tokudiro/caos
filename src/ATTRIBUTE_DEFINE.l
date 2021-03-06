%{
	void getterInHeader(const char* type, const char* asterisk, const char* member)
	{
		fprintf(public_header, "%s", type);
		if (0 != asterisk) fprintf(public_header, "*");
		fprintf(public_header, " %s_get_%s(", SBuf_getStr(class_buf), member);
		fprintf(public_header, "const %s* %s", SBuf_getStr(class_buf), SBuf_getStr(thispointer_buf) );
		fprintf(public_header, ");\n");
	}

	void getterInSource(const char* type, const char* asterisk, const char* member)
	{
		fprintf(source, "%s", type);
		if (0 != asterisk) fprintf(source, "*");
		fprintf(source, " %s_get_%s(", SBuf_getStr(class_buf), member);
		fprintf(source, "const %s* %s", SBuf_getStr(class_buf), SBuf_getStr(thispointer_buf) );
		fprintf(source, ") { return ");
		fprintf(source, "%s->%s", SBuf_getStr(thispointer_buf), member );
		fprintf(source, "; }\n");
	}

	void setterInHeader(const char* type, const char* asterisk, const char* member)
	{
		fprintf(public_header, "void");
		fprintf(public_header, " %s_set_%s(", SBuf_getStr(class_buf), member);
		fprintf(public_header, "%s* %s", SBuf_getStr(class_buf), SBuf_getStr(thispointer_buf) );
		fprintf(public_header, ", ");
		fprintf(public_header, "%s", type);
		if (0 != asterisk) fprintf(public_header, "*");
		fprintf(public_header, " %s);\n", member);
	}

	void setterInSource(const char* type, const char* asterisk, const char* member)
	{
		fprintf(source, "void");
		fprintf(source, " %s_set_%s(", SBuf_getStr(class_buf), member);
		fprintf(source, "%s* %s", SBuf_getStr(class_buf), SBuf_getStr(thispointer_buf) );
		fprintf(source, ", ");
		fprintf(source, "%s", type);
		if (0 != asterisk) fprintf(source, "*");
		fprintf(source, " %s) { ", member);
		fprintf(source, "%s->%s = %s", SBuf_getStr(thispointer_buf), member, member );
		fprintf(source, "; }\n");
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
        if (RBuf_isWSPACE)
        {
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
        BEGIN(ATTRIBUTE_DEFINE_CSA);
	}
	{COMMENT}  {
        if (RBuf_isWSPACE)
        {
            RBuf_allDeque( queue, allque);
            if (isMember){
                if (!isNonDefineHeader) {
                    fprintf( struct_header, "%s", allque);
                }
            }else{
                fprintf( source, "%s", allque);
            }
        }
		if (isOldComment) {
            if (!isNonDefineHeader) {
    			fprintf( struct_header, "/*");
            }
		} else {
            if (!isNonDefineHeader) {
    			fprintf( struct_header, "//");
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
        boolean isType;
	    RBuf_enque(queue, yytext, yyleng, T_SEMICOLON);
	    RBuf_trimQue(queue);
		/* Class Member */
	    countAttribute++;
	    
		isMember = RBuf_back_retype(queue, T_WORD, T_PARAMETER);
		isType = RBuf_back_retype(queue, T_WORD, T_TYPE);
		const char* clazz = RBuf_back_getStr(queue, T_CLASS);
		if (isMember && (isType || 0!=clazz) ) {
			const char* member = RBuf_back_getStr(queue, T_PARAMETER);
			const char* asterisk = RBuf_back_getStr(queue, T_ASTERISK);
			const char* type = RBuf_back_getStr(queue, T_TYPE);
			
			/* getter,setter in header */
			getterInHeader((0!=clazz?clazz:type), asterisk, member);
			setterInHeader((0!=clazz?clazz:type), asterisk, member);
			/* getter,setter in source */
			getterInSource((0!=clazz?clazz:type), asterisk, member);
			setterInSource((0!=clazz?clazz:type), asterisk, member);
		}
        if (!isNonDefineHeader) {
    		fprintf(struct_header, "%s", RBuf_allDeque(queue, allque) );
        }
	}
	\*  { RBuf_enque(queue, yytext, yyleng, T_ASTERISK); }
	.  { RBuf_enque(queue, yytext, yyleng, T_NON); }
}

<ATTRIBUTE_DEFINE_CSA>{
	{COMMENTE} {
        if (!isNonDefineHeader) {
    		fprintf( struct_header, "*/");
        }
		BEGIN(ATTRIBUTE_DEFINE);
	}
	\n {
        if (!isNonDefineHeader) {
            fprintf( struct_header, "\n");
        }
	}
	.  {
        if (!isNonDefineHeader) {
    	    fprintf( struct_header, "%s", yytext);
        }
	}
}

<ATTRIBUTE_DEFINE_CSS>{
	\n {
		if (isOldComment) {
            if (!isNonDefineHeader) {
    			fprintf(struct_header, "*/\n");
            }
		} else {
            if (!isNonDefineHeader) {
    			fprintf(struct_header, "\n");
            }
		}
		BEGIN(ATTRIBUTE_DEFINE);
	}
	.  {
        if (!isNonDefineHeader) {
    	    fprintf(struct_header, "%s", yytext);
        }
	}
}
