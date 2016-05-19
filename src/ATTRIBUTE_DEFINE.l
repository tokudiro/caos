%{
	void getterInHeader(const char* type, const char* asterisk, const char* member)
	{
		fprintf(public_header, "%s", type);
		if (0 != asterisk) fprintf(public_header, "*");
		fprintf(public_header, " %s_get_%s(", SBuf_getStr(class_buf), member);
		if (isThisPointer){
			fprintf(public_header, "const %s* this", SBuf_getStr(class_buf) );
		}else{
			fprintf(public_header, "%s self", SBuf_getStr(class_buf) );
		}
		fprintf(public_header, ");\n");
	}

	void getterInSource(const char* type, const char* asterisk, const char* member)
	{
		fprintf(source, "%s", type);
		if (0 != asterisk) fprintf(source, "*");
		fprintf(source, " %s_get_%s(", SBuf_getStr(class_buf), member);
		if (isThisPointer){
			fprintf(source, "const %s* this", SBuf_getStr(class_buf) );
		}else{
			fprintf(source, "%s self", SBuf_getStr(class_buf) );
		}
		fprintf(source, ") { return ");
		if (isThisPointer){
			fprintf(source, "this->%s", member );
		}else{
			fprintf(source, "self->%s", member );
		}
		fprintf(source, "; }\n");
	}

	void setterInHeader(const char* type, const char* asterisk, const char* member)
	{
		fprintf(public_header, "void");
		fprintf(public_header, " %s_set_%s(", SBuf_getStr(class_buf), member);
		if (isThisPointer){
			fprintf(public_header, "%s* this", SBuf_getStr(class_buf) );
		}else{
			fprintf(public_header, "%s self", SBuf_getStr(class_buf) );
		}
		fprintf(public_header, ", ");
		fprintf(public_header, "%s", type);
		if (0 != asterisk) fprintf(public_header, "*");
		fprintf(public_header, " %s);\n", member);
	}

	void setterInSource(const char* type, const char* asterisk, const char* member)
	{
		fprintf(source, "void");
		fprintf(source, " %s_set_%s(", SBuf_getStr(class_buf), member);
		if (isThisPointer){
			fprintf(source, "%s* this", SBuf_getStr(class_buf) );
		}else{
			fprintf(source, "%s self", SBuf_getStr(class_buf) );
		}
		fprintf(source, ", ");
		fprintf(source, "%s", type);
		if (0 != asterisk) fprintf(source, "*");
		fprintf(source, " %s) { ", member);
		if (isThisPointer){
			fprintf(source, "this->%s = %s", member, member );
		}else{
			fprintf(source, "self->%s = %s", member, member );
		}
		fprintf(source, "; }\n");
	}
%}

<ATTRIBUTE_DEFINE>{
	{NUMBER} {
		RBuf_enque(queue, yytext, yyleng, T_NUMBER);
	}

	{CLASS}  {
		RBuf_enque(queue, &(yytext[1]), yyleng-1, T_CLASS);
		if(isThisPointer) RBuf_enque(queue, "*", 1, T_ASTERISK);
	}

	{DIRECTIVE_ENDCLASS} {
		fprintf( define_header, "};\n" );
		SBuf_init(class_buf);
        if (isVerbose) printf("BEGIN <INITIAL>\n");
		BEGIN(INITIAL);
	}

	{OBJECT} { RBuf_enque(queue, &(yytext[1]), yyleng-1, T_OBJECT); }
	{WORD}   {
		RBuf_enque(queue, yytext, yyleng, T_WORD);
	}
	{WSPACE}   { RBuf_enque(queue, yytext, yyleng, T_WSPACE); }
	{COMMENTS} { fprintf( define_header, "/*"); BEGIN(ATTRIBUTE_DEFINE_CSA); }
	{COMMENT}  {
		if (isOldComment) {
			fprintf( define_header, "/*");
		} else {
			fprintf( define_header, "//");
		}
		BEGIN(ATTRIBUTE_DEFINE_CSS);
	}
	\n {
		fprintf( define_header, "%s", RBuf_allque(queue, allque, SBuf_getStr(class_buf)) );
		fprintf( define_header, "\n");
		if (isMember) { isMember = FALSE; }
        if (isVerbose) printf("BEGIN <CLASS_SCOPE>\n");
		BEGIN(CLASS_SCOPE);
	}
	;  {
	    RBuf_enque(queue, yytext, yyleng, T_SEMICOLON);
	    RBuf_trimque(queue);
		/* Class Member */
	    countAttribute++;
	    
		isMember = RBuf_back_retype(queue, T_WORD, T_PARAMETER);
		boolean isType = RBuf_back_retype(queue, T_WORD, T_TYPE);
		const char* clazz = RBuf_back_getStr(queue, T_CLASS);
		if (isMember && (isType || 0!=clazz) ) {
			const char* member = RBuf_back_getStr(queue, T_PARAMETER);
			const char* asterisk = RBuf_back_getStr(queue, T_ASTERISK);
			const char* type = RBuf_back_getStr(queue, T_TYPE);
			
			// getter,setter in header
			getterInHeader((0!=clazz?clazz:type), asterisk, member);
			setterInHeader((0!=clazz?clazz:type), asterisk, member);
			// getter,setter in source
			getterInSource((0!=clazz?clazz:type), asterisk, member);
			setterInSource((0!=clazz?clazz:type), asterisk, member);
		}
		fprintf(define_header, "%s", RBuf_allque(queue, allque, SBuf_getStr(class_buf)) );
	}
	\*  { RBuf_enque(queue, yytext, yyleng, T_ASTERISK); }
	.  { RBuf_enque(queue, yytext, yyleng, T_NON); }
}

<ATTRIBUTE_DEFINE_CSA>{
	{COMMENTE} {
		fprintf( define_header, "*/");
		BEGIN(ATTRIBUTE_DEFINE);
	}
	\n { fprintf( define_header, "\n"); }
	.  { fprintf( define_header, "%s", yytext); }
}

<ATTRIBUTE_DEFINE_CSS>{
	\n {
		if (isOldComment) {
			fprintf(define_header, "*/\n");
		} else {
			fprintf(define_header, "\n");
		}
		BEGIN(ATTRIBUTE_DEFINE);
	}
	.  { fprintf(define_header, "%s", yytext); }
}
