%{
	void defineClassInSource()
	{
		if (isVerbose) printf("class %s\n", SBuf_getStr(class_buf) );
	    if (classType == TYPE_STRUCT) {
    	    fprintf( source, "#include \"%s\"\n", public_header_name );
	    } else {
            if (!isNonDefineHeader) {
        	    fprintf( source, "#include \"%s\"\n", define_header_name );
            }else{
        	    fprintf( source, "#include \"%s\"\n", public_header_name );
            }
	    }
        if (!isNonPrivateHeader) {
    		fprintf( source, "#include \"%s\"\n", private_header_name );
        }
		if (classType == TYPE_SINGLETON) {
			fprintf( source, "static struct %s_struct instance;\n", SBuf_getStr(class_buf) );
			fprintf( source, "%s", SBuf_getStr(class_buf) );
			if (isThisPointer) fprintf( source, "*");
			fprintf( source, " %s_getInstance() { return &instance; }\n", SBuf_getStr(class_buf) );
		}
		if (isLineNo) fprintf(source, "#line %d \"%s\"\n", yylineno, input_filename);
	}

	void defineClassInHeader()
	{
		if (isLineNo) fprintf(public_header, "#line %d \"%s\"\n", yylineno, input_filename);
	    if (classType == TYPE_STRUCT) {
            fprintf( public_header, "typedef struct %s", SBuf_getStr(class_buf) );
        } else {
            fprintf( public_header, "typedef struct %s_struct", SBuf_getStr(class_buf) );
	    }
		if (!isThisPointer) fprintf( public_header, "*");
		fprintf( public_header, " %s;\n", SBuf_getStr(class_buf) );
		if (classType == TYPE_SINGLETON) {
			fprintf( public_header, "%s", SBuf_getStr(class_buf) );
			if (isThisPointer) fprintf( public_header, "*");
			fprintf( public_header, " %s_getInstance();\n", SBuf_getStr(class_buf) );
		}

	    if (classType != TYPE_STRUCT) {
            if (!isNonPrivateHeader) {
                fprintf( define_header, "#include \"%s\"\n", public_header_name );
	        }
	    }
	    if (isLineNo) {
            if (!isNonDefineHeader) {
                fprintf(struct_header, "#line %d \"%s\"\n", yylineno, input_filename);
            }
	    }
	    if (classType == TYPE_STRUCT) {
            if (!isNonDefineHeader) {
                fprintf( struct_header, "struct %s{\n", SBuf_getStr(class_buf) );
            }
        }else{
            if (!isNonDefineHeader) {
                fprintf( struct_header, "struct %s_struct{\n", SBuf_getStr(class_buf) );
            }
        }
	}
%}

<CLASS_DEFINE>{
	{WORD}  {
		SBuf_setBuf( class_buf, yytext, yyleng);
	    RBuf_setClass(queue, SBuf_getStr(class_buf));
		defineClassInSource();
		defineClassInHeader();
	    if (isVerbose) printf("BEGIN <CLASS_SCOPE>\n");
		BEGIN(CLASS_SCOPE);
	}
	
	{CLASS}  {
		SBuf_setBuf( class_buf, &(yytext[1]), yyleng-1);
	    RBuf_setClass(queue, SBuf_getStr(class_buf));
		defineClassInSource();
		defineClassInHeader();
	    if (isVerbose) printf("BEGIN <CLASS_SCOPE>\n");
		BEGIN(CLASS_SCOPE);
	}

	\n { fprintf( source, "\n" ); }
}
