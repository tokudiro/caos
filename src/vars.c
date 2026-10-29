#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "iface.h"
#include "proto.h"
#include "str.h"
#include "vars.h"

/* ---- variables ---- */

typedef struct { char* name; VarInfo info; } Var;
typedef struct { Var* v; int n; } VarTable;

static char** classes = 0;
static int classCount = 0;
static VarTable locals = { 0, 0 };
static VarTable members = { 0, 0 };

static void silent(const char* file, int line, const char* msg)
{
    (void)file; (void)line; (void)msg;
}

void Vars_addClass(const char* name)
{
    int i;
    for (i = 0; i < classCount; i++) if (strcmp(classes[i], name) == 0) return;
    classes = (char**)realloc(classes, sizeof(char*) * (classCount + 1));
    if (classes == 0) { fprintf(stderr, "error: out of memory\n"); exit(1); }
    classes[classCount++] = Str_dupn(name, strlen(name));
}

int Vars_isClass(const char* name)
{
    int i;
    for (i = 0; i < classCount; i++) if (strcmp(classes[i], name) == 0) return 1;
    return Iface_isInterface(name);
}

void Vars_importClass(const char* name, const char* input)
{
    if (Iface_find(name, input, 0, silent) == 0) Vars_addClass(name);
}

static void clearTable(VarTable* t)
{
    int i;
    for (i = 0; i < t->n; i++) free(t->v[i].name);
    free(t->v);
    t->v = 0;
    t->n = 0;
}

void Vars_clear(void) { clearTable(&locals); }
void Vars_clearMembers(void) { clearTable(&members); }

static void addVar(VarTable* t, const char* name, const char* cls, int stars)
{
    int i;
    Var* v = 0;
    for (i = 0; i < t->n; i++) if (strcmp(t->v[i].name, name) == 0) v = &t->v[i];
    if (v == 0) {
        t->v = (Var*)realloc(t->v, sizeof(Var) * (t->n + 1));
        if (t->v == 0) { fprintf(stderr, "error: out of memory\n"); exit(1); }
        v = &t->v[t->n++];
        v->name = Str_dupn(name, strlen(name));
    }
    snprintf(v->info.cls, sizeof(v->info.cls), "%s", cls);
    v->info.stars = stars;
    v->info.isInterface = Iface_isInterface(cls);
}

static void removeVar(VarTable* t, const char* name)
{
    int i;
    for (i = 0; i < t->n; i++) {
        if (strcmp(t->v[i].name, name) == 0) {
            free(t->v[i].name);
            t->v[i] = t->v[--t->n];
            return;
        }
    }
}

/* a variable is only known if its type is a class with at most one '*' */
static void declare(VarTable* t, const char* name, const char* cls, int stars)
{
    if (Vars_isClass(cls) && stars <= 1) addVar(t, name, cls, stars);
    else removeVar(t, name);    /* a new declaration hides the old one */
}

void Vars_declareProto(const char* proto, int skipThis)
{
    IMethod m;
    char msg[PROTO_MSG_SIZE];
    int i;
    if (Proto_parse(proto, &m, msg)) return;
    for (i = skipThis ? 1 : 0; i < m.nparams; i++) {
        const char* t = m.params[i].type;
        char cls[256];
        int stars = 0;
        size_t l = 0;
        while (strncmp(t, "const ", 6) == 0 || strncmp(t, "struct ", 7) == 0) t = strchr(t, ' ') + 1;
        while (isalnum((unsigned char)t[l]) || t[l] == '_') l++;
        if (l == 0 || l >= sizeof(cls)) continue;
        memcpy(cls, t, l);
        cls[l] = 0;
        for (t += l; *t == '*'; t++) stars++;
        if (*t) continue;
        declare(&locals, m.params[i].name, cls, stars);
    }
    Proto_free(&m);
}

/* "... Class [*] name" at the end of the text, followed by ';' '=' ',' ')' or nothing */
static void declareTail(VarTable* t, const char* text)
{
    size_t e = strlen(text), b;
    char name[256], cls[256];
    int stars = 0;
    while (e > 0 && (isspace((unsigned char)text[e-1]))) e--;
    if (e > 0 && strchr(";=,)", text[e-1])) e--;
    while (e > 0 && (isspace((unsigned char)text[e-1]))) e--;
    b = e;
    while (b > 0 && (isalnum((unsigned char)text[b-1]) || text[b-1] == '_')) b--;
    if (b == e || e - b >= sizeof(name) || isdigit((unsigned char)text[b])) return;
    memcpy(name, text + b, e - b);
    name[e - b] = 0;
    e = b;
    while (e > 0 && (isspace((unsigned char)text[e-1]) || text[e-1] == '*')) {
        if (text[e-1] == '*') stars++;
        e--;
    }
    b = e;
    while (b > 0 && (isalnum((unsigned char)text[b-1]) || text[b-1] == '_')) b--;
    if (b == e || e - b >= sizeof(cls)) return;
    if (b > 0 && (text[b-1] == '.' || text[b-1] == '>' || text[b-1] == '$')) return;
    memcpy(cls, text + b, e - b);
    cls[e - b] = 0;
    declare(t, name, cls, stars);
}

void Vars_declareText(const char* text) { declareTail(&locals, text); }
void Vars_declareMember(const char* text) { declareTail(&members, text); }

static int findIn(const VarTable* t, const char* name, VarInfo* out)
{
    int i;
    for (i = 0; i < t->n; i++) {
        if (strcmp(t->v[i].name, name) == 0) { *out = t->v[i].info; return 1; }
    }
    return 0;
}

int Vars_find(const char* name, VarInfo* out) { return findIn(&locals, name, out); }
int Vars_findMember(const char* name, VarInfo* out) { return findIn(&members, name, out); }

int Vars_resolveCall(const char* text, int isMember, const char* thisName,
                     char* call, size_t size, size_t* keep, const char** error)
{
    const char* p = text + (isMember ? 1 : 0);
    char name[256], method[256], arg[2 * 256 + 8];
    size_t n = 0;
    VarInfo v;
    int arrow;

    *error = 0;
    while (Str_isWordChar(*p) && n < sizeof(name) - 1) name[n++] = *p++;
    name[n] = 0;
    *keep = (size_t)(p - text);
    while (*p == ' ' || *p == '\t') p++;
    arrow = (*p == '-');
    p += arrow ? 2 : 1;
    while (*p == ' ' || *p == '\t') p++;
    n = 0;
    while (Str_isWordChar(*p) && n < sizeof(method) - 1) method[n++] = *p++;
    method[n] = 0;

    if (!(isMember ? Vars_findMember(name, &v) : Vars_find(name, &v))) return 0;
    if (isMember) snprintf(arg, sizeof(arg), "%s->%s", thisName, name);
    else snprintf(arg, sizeof(arg), "%s", name);

    if (v.isInterface) {
        /* an interface is a pair of pointers, passed by value */
        if (arrow) snprintf(call, size, "%s_%s(*%s", v.cls, method, arg);
        else snprintf(call, size, "%s_%s(%s", v.cls, method, arg);
    } else if (v.stars == 1) {
        if (!arrow) *error = "a pointer calls a method with '->'";
        snprintf(call, size, "%s_%s(%s", v.cls, method, arg);
    } else {
        if (arrow) *error = "a value calls a method with '.'";
        snprintf(call, size, "%s_%s(&%s", v.cls, method, arg);
    }
    return 1;
}
