<<EOF>> {
	switch (YY_START) {
	case INITIAL:
		break;
	case CLASS_DEFINE:
		reportError("class name is missing");
		break;
	case TEMPLATE_HEAD:
	case TEMPLATE_SCOPE:
		reportErrorAt(templateLine, "template is not closed (#endclass is missing)");
		break;
	case INSTANCE_DEFINE:
		reportError("#instance is incomplete");
		break;
	case INTERFACE_DEFINE:
		reportError("interface name is missing");
		break;
	case INTERFACE_SCOPE:
		reportErrorAt(interfaceLine, "interface is not closed (#endinterface is missing)");
		break;
	case CLASS_DEFINE_TAIL:
	case CLASS_IMPLEMENTS:
	case CLASS_SCOPE:
	case CLASS_SCOPE_CS:
	case ATTRIBUTE_DEFINE:
		reportErrorAt(classLine, "class is not closed (#endclass is missing)");
		break;
	case METHOD_DEFINE:
	case METHOD_SCOPE:
	case METHOD_SCOPE_CS:
		reportErrorAt(methodLine, "method is not closed ('}' is missing)");
		break;
	case INITIAL_CSA:
	case CLASS_SCOPE_CSA:
	case METHOD_DEFINE_CSA:
	case METHOD_SCOPE_CSA:
	case ATTRIBUTE_DEFINE_CSA:
		reportError("comment is not closed ('*/' is missing)");
		break;
	case INITIAL_ST:
	case METHOD_SCOPE_ST:
		reportError("string is not closed ('\"' is missing)");
		break;
	default:
		break;
	}
	if (isExpanding) {
		/* the end of an expanded template : go on with the file that has #instance */
		yy_delete_buffer(YY_CURRENT_BUFFER);
		yy_switch_to_buffer((YY_BUFFER_STATE)savedBuffer);
		yylineno = savedLine;
		input_filename = savedFile;
		isExpanding = FALSE;
		if (isLineNo) fprintf(source, "#line %d \"%s\"\n", yylineno, input_filename);
		BEGIN(INITIAL);
	} else {
		if (isVerbose) printf("\nEND\n");
		return 0;
	}
}
