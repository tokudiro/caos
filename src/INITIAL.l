<INITIAL>{
	{CLASS}  {
		fprintf(source, "%s", &(yytext[1]));
		if (isThisPointer) fprintf(source, "*");
	}

	{NUMBER} { fprintf(source, "%s", yytext); }

	{OBJECT} { fprintf(source, "%s", &(yytext[1])); }
	{WORD}   { fprintf(source, "%s", yytext); }
	{WSPACE} { fprintf(source, "%s", yytext); }
	{COMMENTS} { fprintf(source, "/*"); BEGIN(INITIAL_CSA); }
	{COMMENT}  {
		if (isOldComment) {
			fprintf(source, "/*"); BEGIN(INITIAL_CSS);
		} else {
			fprintf(source, "//"); BEGIN(INITIAL_CSS);
		}
	}
	\"  { fprintf( source, "\""); BEGIN(INITIAL_ST); }
  
	{DIRECTIVE_IMPORT} {
		fprintf(source, "#include ");
		fprintf(define_header, "#include ");
		BEGIN(IMPORT);
	}
	{DIRECTIVE_CLASS} {
	    if (isVerbose) printf("BEGIN <CLASS_DEFINE>\n");
	    BEGIN(CLASS_DEFINE);
	}
	{DIRECTIVE_SINGLETON} {
	    if (isVerbose) printf("#directive singleton\n");
		singleton = TRUE;
	    if (isVerbose) printf("BEGIN <CLASS_DEFINE>\n");
		BEGIN(CLASS_DEFINE);
	}
	
	\n { fprintf(source, "\n"); }
	.  { fprintf(source, "%s", yytext); }
}

<INITIAL_CSA>{
	{COMMENTE} { fprintf(source, "*/"); BEGIN(INITIAL); }
	\n { fprintf(source, "\n"); }
	. { fprintf(source, "%s", yytext); }
}

<INITIAL_CSS>{
	\n {
		if (isOldComment) {
			fprintf(source, "*/\n"); BEGIN(INITIAL);
		} else {
			fprintf(source, "\n"); BEGIN(INITIAL);
		}
	}
	. { fprintf(source, "%s", yytext); }
}

<INITIAL_ST>{
	\\\" { fprintf( source, "\\\""); }
	\" { fprintf( source, "\""); BEGIN(INITIAL); }
	\n { fprintf(source, "\n"); }
	. { fprintf( source, "%s", yytext); }
}
