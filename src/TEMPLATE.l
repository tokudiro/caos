<CLASS_DEFINE>{CLASS}/"<" {
	snprintf(templateName, sizeof(templateName), "%s", &(yytext[1]));
	templateLine = yylineno;
	templateHeadLen = 0;
	templateHead[0] = 0;
	BEGIN(TEMPLATE_HEAD);
}

<TEMPLATE_HEAD>{
	\n {
		Template_textBegin();
		BEGIN(TEMPLATE_SCOPE);
	}
	. { appendLimited(templateHead, &templateHeadLen, yytext); }
}

<TEMPLATE_SCOPE>{
	{DIRECTIVE_ENDCLASS} {
		finishTemplate(yytext);
		BEGIN(INITIAL);
	}
	.|\n {
		Template_textPut(yytext, yyleng);
	}
}

<INSTANCE_DEFINE>{
	\n {
		int line = 0;
		const char* path = 0;
		char* text;
		instanceText[instanceLen] = 0;
		text = Template_instance(instanceText, input_filename, yylineno - 1, ifaceError, &line, &path);
		if (text) {
			PUSH_INPUT(text, line, path ? path : input_filename, INITIAL, FALSE);
			free(text);
			if (path) {
				addSourceDep(path);
			}
		}
		BEGIN(INITIAL);
	}
	. { appendLimited(instanceText, &instanceLen, yytext); }
}
