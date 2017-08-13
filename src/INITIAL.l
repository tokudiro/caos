<INITIAL>{
	{CLASS}  {
		fprintf((isOutputHeader?public_header:source), "%s", &(yytext[1]));
		fprintf((isOutputHeader?public_header:source), "*");
	}

	{NUMBER} { fprintf((isOutputHeader?public_header:source), "%s", yytext); }

	{OBJECT} { fprintf((isOutputHeader?public_header:source), "%s", &(yytext[1])); }
	{WORD}   { fprintf((isOutputHeader?public_header:source), "%s", yytext); }
	{WSPACE} { fprintf((isOutputHeader?public_header:source), "%s", yytext); }
	{COMMENTS} { fprintf((isOutputHeader?public_header:source), "/*"); BEGIN(INITIAL_CSA); }
	{COMMENT}  {
		if (isOldComment) {
			fprintf((isOutputHeader?public_header:source), "/*"); BEGIN(INITIAL_CSS);
		} else {
			fprintf((isOutputHeader?public_header:source), "//"); BEGIN(INITIAL_CSS);
		}
	}
    {DIRECTIVE_COMMENT} {
		BEGIN(INITIAL_CS);
    }
	\"  { fprintf((isOutputHeader?public_header:source), "\""); BEGIN(INITIAL_ST); }
  
	{DIRECTIVE_IMPORT} {
		fprintf((isOutputHeader?public_header:source), "#include ");
        if (!isNonPrivateHeader) {
    		fprintf(define_header, "#include ");
        }
		BEGIN(IMPORT);
	}
	{DIRECTIVE_CLASS} {
	    if (isVerbose) printf("BEGIN <CLASS_DEFINE>\n");
	    BEGIN(CLASS_DEFINE);
	}
    {DIRECTIVE_HEADER} {
        if (isVerbose) printf("output header start\n");
        isOutputHeader = TRUE;
    }
    {DIRECTIVE_ENDHEADER} {
        if (isVerbose) printf("output header end\n");
        isOutputHeader = FALSE;
    }
	{DIRECTIVE_SINGLETON} {
	    if (isVerbose) printf("#directive singleton\n");
		classType = TYPE_SINGLETON;
	    if (isVerbose) printf("BEGIN <CLASS_DEFINE>\n");
		BEGIN(CLASS_DEFINE);
	}
	{DIRECTIVE_STRUCT} {
	    if (isVerbose) printf("#directive struct\n");
		classType = TYPE_STRUCT;
	    if (isVerbose) printf("BEGIN <CLASS_DEFINE>\n");
		BEGIN(CLASS_DEFINE);
	}
	{DIRECTIVE_UTILITY} {
	    if (isVerbose) printf("#directive utility\n");
		classType = TYPE_UTILITY;
	    if (isVerbose) printf("BEGIN <CLASS_DEFINE>\n");
		BEGIN(CLASS_DEFINE);
	}
	
	\n { fprintf((isOutputHeader?public_header:source), "\n"); }
	.  { fprintf((isOutputHeader?public_header:source), "%s", yytext); }
}

<INITIAL_CSA>{
	{COMMENTE} { fprintf((isOutputHeader?public_header:source), "*/"); BEGIN(INITIAL); }
	\n { fprintf((isOutputHeader?public_header:source), "\n"); }
	. { fprintf((isOutputHeader?public_header:source), "%s", yytext); }
}

<INITIAL_CSS>{
	\n {
		if (isOldComment) {
			fprintf((isOutputHeader?public_header:source), "*/\n"); BEGIN(INITIAL);
		} else {
			fprintf((isOutputHeader?public_header:source), "\n"); BEGIN(INITIAL);
		}
	}
	. { fprintf((isOutputHeader?public_header:source), "%s", yytext); }
}

<INITIAL_ST>{
	\\\" { fprintf((isOutputHeader?public_header:source), "\\\""); }
	\" { fprintf((isOutputHeader?public_header:source), "\""); BEGIN(INITIAL); }
	\n { fprintf((isOutputHeader?public_header:source), "\n"); }
	. { fprintf((isOutputHeader?public_header:source), "%s", yytext); }
}

<INITIAL_CS>{
	\n {
		BEGIN(INITIAL);
	}
    . {;}
}
