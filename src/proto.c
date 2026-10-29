#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "proto.h"
#include "str.h"

static int isKeyword(const char* w, size_t n)
{
    static const char* kw[] = { "int", "char", "short", "long", "float", "double",
                                "void", "unsigned", "signed", "const", "struct",
                                "enum", "union", "volatile", 0 };
    int i;
    for (i = 0; kw[i]; i++) {
        if (strlen(kw[i]) == n && strncmp(kw[i], w, n) == 0) return 1;
    }
    return 0;
}

/* "$Name" -> "Name*" ; comments removed ; the result is malloc'ed */
char* Proto_clean(const char* s)
{
    size_t n = strlen(s), i = 0, o = 0;
    char* out = Str_alloc(n * 2 + 1);
    while (i < n) {
        if (s[i] == '/' && s[i+1] == '*') {
            i += 2;
            while (i < n && !(s[i] == '*' && s[i+1] == '/')) i++;
            i += 2;
            out[o++] = ' ';
        } else if (s[i] == '/' && s[i+1] == '/') {
            while (i < n && s[i] != '\n') i++;
        } else if (s[i] == '$' && (isalpha((unsigned char)s[i+1]) || s[i+1] == '_')) {
            i++;
            while (i < n && (isalnum((unsigned char)s[i]) || s[i] == '_')) out[o++] = s[i++];
            out[o++] = '*';
        } else {
            out[o++] = s[i++];
        }
    }
    out[o] = 0;
    return out;
}

/* ---- prototype parser : "RET name(TYPE [name], ...)" ---- */

void Proto_free(IMethod* m)
{
    int i;
    free(m->ret);
    free(m->name);
    for (i = 0; i < m->nparams; i++) { free(m->params[i].type); free(m->params[i].name); }
    free(m->params);
}

/* returns 0, or fills msg */
int Proto_parse(const char* decl, IMethod* m, char* msg)
{
    char* text = Proto_clean(decl);
    const char* lp = strchr(text, '(');
    const char* rp = strrchr(text, ')');
    const char* nameEnd;
    const char* nameBeg;
    const char* p;
    const char* argBeg;
    int index = 0;

    memset(m, 0, sizeof(*m));
    if (lp == 0 || rp == 0 || rp < lp) {
        snprintf(msg, PROTO_MSG_SIZE, "cannot read the method declaration: %s", decl);
        free(text);
        return 1;
    }
    nameEnd = lp;
    while (nameEnd > text && isspace((unsigned char)nameEnd[-1])) nameEnd--;
    nameBeg = nameEnd;
    while (nameBeg > text && (isalnum((unsigned char)nameBeg[-1]) || nameBeg[-1] == '_')) nameBeg--;
    if (nameBeg == nameEnd) {
        snprintf(msg, PROTO_MSG_SIZE, "method name is missing: %s", decl);
        free(text);
        return 1;
    }
    m->name = Str_dupn(nameBeg, nameEnd - nameBeg);
    m->ret = Str_normalize(text, nameBeg - text);

    /* the parameters */
    argBeg = lp + 1;
    for (p = argBeg; p <= rp; p++) {
        const char* q;
        const char* end;
        const char* e;
        IParam* np;
        if (p < rp) {
            if (*p != ',') {
                if (*p == '(' || *p == '[') {
                    snprintf(msg, PROTO_MSG_SIZE, "a function pointer or array parameter is not supported: %s", decl);
                    free(text);
                    Proto_free(m);
                    return 1;
                }
                continue;
            }
        }
        end = p;
        q = argBeg;
        argBeg = p + 1;
        {
            char* t = Str_normalize(q, end - q);
            if (t[0] == 0 || strcmp(t, "void") == 0) {
                free(t);
                if (p == rp && m->nparams == 0) continue;
                snprintf(msg, PROTO_MSG_SIZE, "a parameter is missing: %s", decl);
                free(text);
                Proto_free(m);
                return 1;
            }
            free(t);
        }
        /* the name : the last word, if it is not a type word and something precedes it */
        e = end;
        while (e > q && isspace((unsigned char)e[-1])) e--;
        nameEnd = e;
        nameBeg = e;
        while (nameBeg > q && (isalnum((unsigned char)nameBeg[-1]) || nameBeg[-1] == '_')) nameBeg--;
        m->params = (IParam*)realloc(m->params, sizeof(IParam) * (m->nparams + 1));
        if (m->params == 0) { fprintf(stderr, "error: out of memory\n"); exit(1); }
        np = &m->params[m->nparams++];
        if (nameBeg < nameEnd && nameBeg > q && !isKeyword(nameBeg, nameEnd - nameBeg)
            && !isdigit((unsigned char)*nameBeg)) {
            np->name = Str_dupn(nameBeg, nameEnd - nameBeg);
            np->type = Str_normalize(q, nameBeg - q);
        } else {
            char gen[16];
            snprintf(gen, sizeof(gen), "a%d", index);
            np->name = Str_dupn(gen, strlen(gen));
            np->type = Str_normalize(q, e - q);
        }
        index++;
    }
    free(text);
    return 0;
}

/* "RET name(T1, T2)" for messages */
void Proto_describe(const IMethod* m, int from, char* out, size_t size)
{
    int i;
    size_t o = (size_t)snprintf(out, size, "%s %s(", m->ret, m->name);
    for (i = from; i < m->nparams && o < size; i++) {
        o += (size_t)snprintf(out + o, size - o, "%s%s", i > from ? ", " : "", m->params[i].type);
    }
    if (o < size) snprintf(out + o, size - o, ")");
}

void Proto_returnType(const char* proto, char* out, size_t size)
{
    IMethod m;
    char msg[PROTO_MSG_SIZE];
    out[0] = 0;
    if (Proto_parse(proto, &m, msg)) return;
    snprintf(out, size, "%s", m.ret);
    Proto_free(&m);
}
