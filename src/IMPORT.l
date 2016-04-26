<IMPORT>{
	{CLASS}  {
		fprintf(source, "\"%s.h\"\n", &(yytext[1]));
		fprintf(define_header, "\"%s.h\"\n", &(yytext[1]));
		BEGIN(INITIAL);
	}

	{WSPACE} {
		fprintf(source, "%s", yytext);
		fprintf(define_header, "%s", yytext);
	}

	\n {
		fprintf(source, "\n");
		fprintf(define_header, "\n");
	}
	.  {
		fprintf(source, "%s", yytext);
		fprintf(define_header, "%s", yytext);
	}
}
