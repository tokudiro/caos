<IMPORT>{
	{CLASS}  {
		fprintf(source, "\"%s.h\"", &(yytext[1]));
		BEGIN(INITIAL);
	}

	{WSPACE} {
		fprintf(source, "%s", yytext);
	}

	\n { fprintf(source, "\n"); }
	.  { fprintf(source, "%s", yytext); }
}
