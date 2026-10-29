<INTERFACE_DEFINE>{
	{WSPACE} {;}
	{CLASS} {
		snprintf(interfaceName, sizeof(interfaceName), "%s", &(yytext[1]));
		Iface_textBegin();
		BEGIN(INTERFACE_SCOPE);
	}
	{WORD} {
		snprintf(interfaceName, sizeof(interfaceName), "%s", yytext);
		Iface_textBegin();
		BEGIN(INTERFACE_SCOPE);
	}
	\n {
		reportErrorAt(yylineno - 1, "interface name is missing");
		BEGIN(INITIAL);
	}
	. {
		reportError("interface name is missing");
		BEGIN(INITIAL);
	}
}

<INTERFACE_SCOPE>{
	{DIRECTIVE_ENDINTERFACE} {
		finishInterface();
		BEGIN(INITIAL);
	}
	.|\n {
		Iface_textPut(yytext, yyleng);
	}
}
