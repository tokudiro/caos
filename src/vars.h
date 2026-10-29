#ifndef VARS_H__
#define VARS_H__
/*
 * Variables : the declared type of the variables of the method being
 * translated and of the members of the class, for "x->method()" and
 * "v.method()".  Only a type that is a caos class (or an interface) is kept.
 * Written in C (not in caos): it is a part of the translator only.
 */
#include <stddef.h>

typedef struct {
    char cls[256];
    int stars;          /* 0 : a value, 1 : a pointer */
    int isInterface;
} VarInfo;

/* the classes the translator knows : this file's, the imported ones, the interfaces */
void Vars_addClass(const char* name);
int Vars_isClass(const char* name);
/* "#import $Name" : the interface in Name.caos, or a class named Name */
void Vars_importClass(const char* name, const char* input);

void Vars_clear(void);
/* the parameters of a prototype "R Class_m(Class* pthis, T a)" ; skipThis drops the first one */
void Vars_declareProto(const char* proto, int skipThis);
/* a declaration at the end of the text : "int a; Foo* p" */
void Vars_declareText(const char* text);
void Vars_declareMember(const char* text);
void Vars_clearMembers(void);
int Vars_find(const char* name, VarInfo* out);
int Vars_findMember(const char* name, VarInfo* out);

/*
 * "var->method(" / "v.method(" / "@member->method(" : the call "Class_method(arg"
 * for the class of the variable. 0 if the variable is not of a class (nothing is
 * to be changed then) ; *keep is the length of the variable at the start of the
 * text. *error gets a message (or 0) when the operator does not fit the type.
 */
int Vars_resolveCall(const char* text, int isMember, const char* thisName,
                     char* call, size_t size, size_t* keep, const char** error);

#endif /* VARS_H__ */
