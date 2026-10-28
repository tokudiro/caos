<<EOF>> {
	switch (YY_START) {
	case INITIAL:
		break;
	case CLASS_DEFINE:
		reportError("class name is missing");
		break;
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
	if (isVerbose) printf("\nEND\n");
	return 0;
}
