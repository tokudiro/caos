#ifndef IFACE_H__
#define IFACE_H__
/*
 * Interfaces : a table of the interfaces and of the public methods of the
 * class being translated, used to check "implements" and to generate the
 * function pointer tables.  Written in C (not in caos): it is a part of the
 * translator only.  See doc/language.md.
 */
#include <stdio.h>

typedef struct { char* type; char* name; } IParam;
typedef struct {
    char* ret;
    char* name;
    IParam* params;
    int nparams;
} IMethod;
typedef struct {
    char* name;
    char* path;     /* the .caos file it was read from; 0 : the current input */
    IMethod* methods;
    int nmethods;
} Iface;

/* helpers shared with template.c : malloc'ed copy / zeroed buffer (exit on failure) */
char* Str_dupn(const char* s, size_t n);
char* Str_alloc(size_t n);

/* error report : the translator counts it and prints "file:line: error: msg" */
typedef void (*IfaceErr)(const char* file, int line, const char* msg);

/* #interface ... #endinterface : collect the text, then register it */
void Iface_textBegin(void);
void Iface_textPut(const char* str, int len);
Iface* Iface_define(const char* name, const char* file, int line, IfaceErr err);

/* find a registered interface, or read it from <folder of input>/<name>.caos */
Iface* Iface_find(const char* name, const char* input, int line, IfaceErr err);

/* the return type of a prototype ("" if it cannot be read) */
void Iface_returnType(const char* proto, char* out, size_t size);

void Iface_emitHeader(const Iface* iface, FILE* header);
void Iface_emitSource(const Iface* iface, FILE* source);

/* the class being translated */
void Iface_classBegin(void);
void Iface_classMethod(const char* proto);
void Iface_classImplements(Iface* iface);
int Iface_classImplementsCount(void);
/* check the methods against every interface, and write the tables */
void Iface_classFinish(const char* cls, const char* file, int line, FILE* source, FILE* header, IfaceErr err);

/*
 * Variables : the declared type of the variables of the method being
 * translated and of the members of the class, for "x->method()" and
 * "v.method()".  Only a type that is a caos class (or an interface) is kept.
 */
typedef struct {
    char cls[256];
    int stars;          /* 0 : a value, 1 : a pointer */
    int isInterface;
} VarInfo;

void Iface_addClass(const char* name);
int Iface_isClass(const char* name);
/* "#import $Name" : the interface in Name.caos, or a class named Name */
void Iface_importClass(const char* name, const char* input);

void Vars_clear(void);
/* the parameters of a prototype "R Class_m(Class* pthis, T a)" ; skipThis drops the first one */
void Vars_declareProto(const char* proto, int skipThis);
/* a declaration at the end of the text : "int a; Foo* p" */
void Vars_declareText(const char* text);
void Vars_declareMember(const char* text);
void Vars_clearMembers(void);
int Vars_find(const char* name, VarInfo* out);
int Vars_findMember(const char* name, VarInfo* out);

#endif /* IFACE_H__ */
