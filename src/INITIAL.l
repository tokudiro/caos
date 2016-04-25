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
	{COMMENT}  { fprintf(source, "/*"); BEGIN(INITIAL_CSS); }
	\"  { fprintf( source, "\""); BEGIN(INITIAL_ST); }
  
	{DIRECTIVE_IMPORT} { fprintf(source, "#include "); BEGIN(IMPORT); }
	{DIRECTIVE_CLASS} { BEGIN(CLASS_DEFINE); }
	\n { fprintf(source, "\n"); }
	.  { fprintf(source, "%s", yytext); }
}

<INITIAL_CSA>{
	{COMMENTE} { fprintf(source, "*/"); BEGIN(INITIAL); }
	. { fprintf(source, "%s", yytext); }
}

<INITIAL_CSS>{
	\n { fprintf(source, "*/\n"); BEGIN(INITIAL); }
	. { fprintf(source, "%s", yytext); }
}

<INITIAL_ST>{
	\\\" { fprintf( source, "\\\""); }
	\" { fprintf( source, "\""); BEGIN(INITIAL); }
	. { fprintf( source, "%s", yytext); }
}
