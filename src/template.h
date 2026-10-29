#ifndef TEMPLATE_H__
#define TEMPLATE_H__
/*
 * Class templates : "#class $List<T>" is kept as text, and "#instance
 * $IntList = $List<int>" feeds the text back to the translator with the type
 * parameters replaced.  Written in C (not in caos): it is a part of the
 * translator only.  See doc/language.md.
 */
#include "iface.h"

typedef struct {
    char* name;
    char** params;
    int nparams;
    char* directive;    /* "#class", "#struct", ... as written */
    char* endDirective; /* "#endclass", ... as written */
    char* tail;         /* the rest of the first line after "<...>" */
    char* body;         /* the lines between the first line and the end directive */
    int line;           /* line of the first line */
    char* path;         /* the .caos file it was read from; 0 : the current input */
} Template;

/* #class $Name<T> ... : collect the body, then register the template */
void Template_textBegin(void);
void Template_textPut(const char* str, int len);
Template* Template_define(const char* name, const char* head, const char* directive,
                          const char* endDirective, int line, const char* file, IfaceErr err);

/*
 * "#instance $Inst = $Name<args>" : the text to scan in place of the line, or 0
 * (the error is reported). *line and *path are where the text comes from.
 */
char* Template_instance(const char* text, const char* input, int lineNo, IfaceErr err,
                        int* line, const char** path);

#endif /* TEMPLATE_H__ */
