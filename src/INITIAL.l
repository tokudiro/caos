<INITIAL>{
	{CLASS}  {
		fprintf((isOutputHeader?public_header:source), "%s", &(yytext[1]));
		fprintf((isOutputHeader?public_header:source), "*");
	}

	{NUMBER} { fprintf((isOutputHeader?public_header:source), "%s", yytext); }

	{OBJECT} {
		reportError("'@' member access outside of a class");
		fprintf((isOutputHeader?public_header:source), "%s", &(yytext[1]));
	}
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
        if (!isNonDefineHeader) {
    		fprintf(define_header, "#include ");
        }
		BEGIN(IMPORT);
	}
	{DIRECTIVE_CLASS} {
	    snprintf(classDirective, sizeof(classDirective), "%s", yytext);
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
	    snprintf(classDirective, sizeof(classDirective), "%s", yytext);
	    if (isVerbose) printf("#directive singleton\n");
		classType = TYPE_SINGLETON;
	    if (isVerbose) printf("BEGIN <CLASS_DEFINE>\n");
		BEGIN(CLASS_DEFINE);
	}
	{DIRECTIVE_STRUCT} {
	    snprintf(classDirective, sizeof(classDirective), "%s", yytext);
	    if (isVerbose) printf("#directive struct\n");
		classType = TYPE_STRUCT;
	    if (isVerbose) printf("BEGIN <CLASS_DEFINE>\n");
		BEGIN(CLASS_DEFINE);
	}
	{DIRECTIVE_UTILITY} {
	    snprintf(classDirective, sizeof(classDirective), "%s", yytext);
	    if (isVerbose) printf("#directive utility\n");
		classType = TYPE_UTILITY;
	    if (isVerbose) printf("BEGIN <CLASS_DEFINE>\n");
		BEGIN(CLASS_DEFINE);
	}
	
	{DIRECTIVE_INVARIANT}.* {
		reportError("#invariant belongs in a class, before its methods");
	}
	{DIRECTIVE_NOINVARIANT}.* {
		reportError("#noinvariant belongs between the header of a method and its '{'");
	}
	{DIRECTIVE_REQUIRE}.* |
	{DIRECTIVE_ENSURE}.*  {
		reportError("#require and #ensure belong between the header of a method and its '{'");
	}
	{DIRECTIVE_INSTANCE} {
	    instanceLen = 0;
	    BEGIN(INSTANCE_DEFINE);
	}
	{DIRECTIVE_INTERFACE} {
	    interfaceLine = yylineno;
	    BEGIN(INTERFACE_DEFINE);
	}
	{DIRECTIVE_ENDINTERFACE} {
		reportError("#endinterface without #interface");
	}
	{DIRECTIVE_ENDCLASS} {
		reportError("#endclass without #class");
	}
	{DIRECTIVE_PUBLIC_METHOD} |
	{DIRECTIVE_PRIVATE_METHOD} |
	{DIRECTIVE_CLASS_METHOD} |
	{DIRECTIVE_PUBLIC_CONST_METHOD} |
	{DIRECTIVE_PUBLIC_ATTRIBUTE} {
		reportError("method or member directive outside of a class");
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
