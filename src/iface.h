#ifndef IFACE_H__
#define IFACE_H__
/*
 * Interfaces : the table of the interfaces, and the check of "implements" with
 * the generation of the function pointer tables.  Written in C (not in caos):
 * it is a part of the translator only.  See doc/language.md.
 */
#include <stdio.h>

#include "proto.h"

typedef struct {
    char* name;
    char* path;     /* the .caos file it was read from; 0 : the current input */
    IMethod* methods;
    int nmethods;
} Iface;

/* error report : the translator counts it and prints "file:line: error: msg" */
typedef void (*IfaceErr)(const char* file, int line, const char* msg);

/* #interface ... #endinterface : collect the text, then register it */
void Iface_textBegin(void);
void Iface_textPut(const char* str, int len);
Iface* Iface_define(const char* name, const char* file, int line, IfaceErr err);

/* a registered interface, or 0 */
Iface* Iface_lookup(const char* name);
#define Iface_isInterface(name) (Iface_lookup(name) != 0)
/* find a registered interface, or read it from <folder of input>/<name>.caos */
Iface* Iface_find(const char* name, const char* input, int line, IfaceErr err);

void Iface_emitHeader(const Iface* iface, FILE* header);
void Iface_emitSource(const Iface* iface, FILE* source);

/* the class being translated */
void Iface_classBegin(void);
void Iface_classMethod(const char* proto);
void Iface_classImplements(Iface* iface);
int Iface_classImplementsCount(void);
/* check the methods against every interface, and write the tables */
void Iface_classFinish(const char* cls, const char* file, int line, FILE* source, FILE* header, IfaceErr err);

#endif /* IFACE_H__ */
