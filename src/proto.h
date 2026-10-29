#ifndef PROTO_H__
#define PROTO_H__
/*
 * Prototypes : "RET name(TYPE [name], ...)" read as text, for the interfaces,
 * the check of "implements" and the types of the variables.
 * Written in C (not in caos): it is a part of the translator only.
 */
#include <stddef.h>

#define PROTO_MSG_SIZE 5120

typedef struct { char* type; char* name; } IParam;
typedef struct {
    char* ret;
    char* name;
    IParam* params;
    int nparams;
} IMethod;

/* "$Name" -> "Name*" and the comments removed ; the result is malloc'ed */
char* Proto_clean(const char* s);
/* 0, or 1 with the reason in msg (PROTO_MSG_SIZE bytes). Types are normalized ; a parameter without name gets "a<N>" */
int Proto_parse(const char* decl, IMethod* m, char* msg);
void Proto_free(IMethod* m);
/* "RET name(T1, T2)" from parameter `from`, for messages */
void Proto_describe(const IMethod* m, int from, char* out, size_t size);
/* the return type of a prototype ("" if it cannot be read) */
void Proto_returnType(const char* proto, char* out, size_t size);

#endif /* PROTO_H__ */
