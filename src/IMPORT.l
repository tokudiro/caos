<IMPORT>{
	{CLASS}  {
		fprintf(source, "\"%s.h\"", &(yytext[1]));
	    if (!isNonDefineHeader) {
    		fprintf(struct_header, "\"%s.h\"\n", &(yytext[1]));
	    }
	    if (isVerbose) printf("BEGIN <INITIAL>\n");
		BEGIN(INITIAL);
	}

	{WSPACE} {
		fprintf(source, "%s", yytext);
	    if (!isNonDefineHeader) {
    		fprintf(struct_header, "%s", yytext);
	    }
	}

	\n {
		fprintf(source, "\n");
	    if (!isNonDefineHeader) {
    		fprintf(struct_header, "\n");
	    }
	}
	.  {
		fprintf(source, "%s", yytext);
	    if (!isNonDefineHeader) {
    		fprintf(struct_header, "%s", yytext);
	    }
	}
}
