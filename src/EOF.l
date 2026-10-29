<<EOF>> {
	if (pushedCount > 0 && pushedInput[pushedCount-1].isInjection) {
		/* the end of the text of a contract : go on with the method */
		POP_INPUT();
	} else {
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
	if (pushedCount > 0) {
		/* the end of an expanded template : go on with the file that has #instance */
		POP_INPUT();
		if (isLineNo) fprintf(source, "#line %d \"%s\"\n", yylineno, input_filename);
	} else {
		if (isVerbose) printf("\nEND\n");
		return 0;
	}
	}
}
