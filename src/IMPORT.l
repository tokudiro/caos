<IMPORT>{
	{CLASS}  {
		{
			char header[MAX_TEXT + 4];
			snprintf(header, sizeof(header), "%s.h", &(yytext[1]));
			addImport(header);
		}
		Iface_importClass(&(yytext[1]), input_filename);
		fprintf(source, "\"%s.h\"", &(yytext[1]));
	    if (!isNonDefineHeader) {
    		fprintf(struct_header, "\"%s.h\"\n", &(yytext[1]));
	    }
	    if (isVerbose) printf("BEGIN <INITIAL>\n");
		BEGIN(INITIAL);
	}

	\"[^"\n]*\" {
		/* #import "x.h" : the file is looked for next to the output */
		yytext[yyleng-1] = 0;
		addImport(&(yytext[1]));
		yytext[yyleng-1] = '"';
		fprintf(source, "%s", yytext);
	    if (!isNonDefineHeader) {
    		fprintf(struct_header, "%s", yytext);
	    }
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
