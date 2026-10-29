#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#include "iface.h"

#define MSG_SIZE 5120

/* ---- small helpers ---- */

char* Str_dupn(const char* s, size_t n)
{
    char* p = (char*)malloc(n + 1);
    if (p == 0) { fprintf(stderr, "error: out of memory\n"); exit(1); }
    memcpy(p, s, n);
    p[n] = 0;
    return p;
}

char* Str_alloc(size_t n)
{
    char* p = (char*)calloc(n + 1, 1);
    if (p == 0) { fprintf(stderr, "error: out of memory\n"); exit(1); }
    return p;
}

/* trim the blanks at both ends, collapse the others, drop the blanks around '*' */
static char* normalize(const char* s, size_t n)
{
    char* out = Str_alloc(n * 2 + 1);
    size_t o = 0, i;
    int pendingSpace = 0;
    for (i = 0; i < n; i++) {
        if (isspace((unsigned char)s[i])) {
            pendingSpace = 1;
            continue;
        }
        if (pendingSpace && o > 0 && out[o-1] != '*' && s[i] != '*') {
            out[o++] = ' ';
        }
        pendingSpace = 0;
        out[o++] = s[i];
    }
    out[o] = 0;
    return out;
}

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
static char* clean(const char* s)
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

static void freeMethod(IMethod* m)
{
    int i;
    free(m->ret);
    free(m->name);
    for (i = 0; i < m->nparams; i++) { free(m->params[i].type); free(m->params[i].name); }
    free(m->params);
}

/* returns 0, or fills msg */
static int parseProto(const char* decl, IMethod* m, char* msg)
{
    char* text = clean(decl);
    const char* lp = strchr(text, '(');
    const char* rp = strrchr(text, ')');
    const char* nameEnd;
    const char* nameBeg;
    const char* p;
    const char* argBeg;
    int index = 0;

    memset(m, 0, sizeof(*m));
    if (lp == 0 || rp == 0 || rp < lp) {
        snprintf(msg, MSG_SIZE, "cannot read the method declaration: %s", decl);
        free(text);
        return 1;
    }
    nameEnd = lp;
    while (nameEnd > text && isspace((unsigned char)nameEnd[-1])) nameEnd--;
    nameBeg = nameEnd;
    while (nameBeg > text && (isalnum((unsigned char)nameBeg[-1]) || nameBeg[-1] == '_')) nameBeg--;
    if (nameBeg == nameEnd) {
        snprintf(msg, MSG_SIZE, "method name is missing: %s", decl);
        free(text);
        return 1;
    }
    m->name = Str_dupn(nameBeg, nameEnd - nameBeg);
    m->ret = normalize(text, nameBeg - text);

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
                    snprintf(msg, MSG_SIZE, "a function pointer or array parameter is not supported: %s", decl);
                    free(text);
                    freeMethod(m);
                    return 1;
                }
                continue;
            }
        }
        end = p;
        q = argBeg;
        argBeg = p + 1;
        {
            char* t = normalize(q, end - q);
            if (t[0] == 0 || strcmp(t, "void") == 0) {
                free(t);
                if (p == rp && m->nparams == 0) continue;
                snprintf(msg, MSG_SIZE, "a parameter is missing: %s", decl);
                free(text);
                freeMethod(m);
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
            np->type = normalize(q, nameBeg - q);
        } else {
            char gen[16];
            snprintf(gen, sizeof(gen), "a%d", index);
            np->name = Str_dupn(gen, strlen(gen));
            np->type = normalize(q, e - q);
        }
        index++;
    }
    free(text);
    return 0;
}

/* "RET name(T1, T2)" for messages */
static void describe(const IMethod* m, int from, char* out, size_t size)
{
    int i;
    size_t o = (size_t)snprintf(out, size, "%s %s(", m->ret, m->name);
    for (i = from; i < m->nparams && o < size; i++) {
        o += (size_t)snprintf(out + o, size - o, "%s%s", i > from ? ", " : "", m->params[i].type);
    }
    if (o < size) snprintf(out + o, size - o, ")");
}

/* ---- the text of an #interface ---- */

static char* text = 0;
static size_t textLen = 0;

void Iface_textBegin(void)
{
    free(text);
    text = Str_alloc(0);
    textLen = 0;
}

void Iface_textPut(const char* str, int len)
{
    char* p = (char*)realloc(text, textLen + len + 1);
    if (p == 0) { fprintf(stderr, "error: out of memory\n"); exit(1); }
    text = p;
    memcpy(text + textLen, str, len);
    textLen += len;
    text[textLen] = 0;
}

static Iface** registry = 0;
static int registryCount = 0;

static Iface* lookup(const char* name)
{
    int i;
    for (i = 0; i < registryCount; i++) {
        if (strcmp(registry[i]->name, name) == 0) return registry[i];
    }
    return 0;
}

Iface* Iface_define(const char* name, const char* file, int line, IfaceErr err)
{
    Iface* iface;
    char* body = clean(text ? text : "");
    char* s = body;
    char msg[MSG_SIZE];
    int i, j;

    if (lookup(name)) {
        snprintf(msg, sizeof(msg), "interface %s is defined twice", name);
        err(file, line, msg);
        free(body);
        return 0;
    }
    iface = (Iface*)calloc(1, sizeof(Iface));
    if (iface == 0) { fprintf(stderr, "error: out of memory\n"); exit(1); }
    iface->name = Str_dupn(name, strlen(name));
    while (*s) {
        char* semi = strchr(s, ';');
        IMethod m;
        char* decl;
        if (semi == 0) {
            decl = normalize(s, strlen(s));
            if (decl[0]) err(file, line, "';' is missing in the interface");
            free(decl);
            break;
        }
        decl = normalize(s, semi - s);
        s = semi + 1;
        if (decl[0] == 0) { free(decl); continue; }
        if (parseProto(decl, &m, msg)) {
            err(file, line, msg);
        } else {
            for (j = 0; j < m.nparams; j++) {
                if (strcmp(m.params[j].name, "r") == 0 || strcmp(m.params[j].name, "self") == 0) {
                    snprintf(msg, sizeof(msg), "the parameter name '%s' of %s::%s is reserved in an interface", m.params[j].name, name, m.name);
                    err(file, line, msg);
                }
            }
            for (i = 0; i < iface->nmethods; i++) {
                if (strcmp(iface->methods[i].name, m.name) == 0) {
                    snprintf(msg, sizeof(msg), "%s::%s is declared twice", name, m.name);
                    err(file, line, msg);
                }
            }
            iface->methods = (IMethod*)realloc(iface->methods, sizeof(IMethod) * (iface->nmethods + 1));
            if (iface->methods == 0) { fprintf(stderr, "error: out of memory\n"); exit(1); }
            iface->methods[iface->nmethods++] = m;
        }
        free(decl);
    }
    free(body);
    if (iface->nmethods == 0) {
        snprintf(msg, sizeof(msg), "interface %s has no method", name);
        err(file, line, msg);
    }
    registry = (Iface**)realloc(registry, sizeof(Iface*) * (registryCount + 1));
    if (registry == 0) { fprintf(stderr, "error: out of memory\n"); exit(1); }
    registry[registryCount++] = iface;
    return iface;
}

/* ---- reading another file ---- */

static int startsWith(const char* s, const char* prefix)
{
    return strncmp(s, prefix, strlen(prefix)) == 0;
}

Iface* Iface_find(const char* name, const char* input, int line, IfaceErr err)
{
    Iface* iface = lookup(name);
    const char* slash;
    char path[4096];
    char buf[1024];
    char msg[MSG_SIZE];
    FILE* fp;
    int inside = 0, startLine = 0, n = 0;

    if (iface) return iface;
    slash = strrchr(input, '/');
    snprintf(path, sizeof(path), "%.*s%s.caos", slash ? (int)(slash - input + 1) : 0, input, name);
    fp = fopen(path, "r");
    if (fp == 0) {
        snprintf(msg, sizeof(msg), "interface %s is not defined (define it before the class, or in %s)", name, path);
        err(input, line, msg);
        return 0;
    }
    Iface_textBegin();
    while (fgets(buf, sizeof(buf), fp)) {
        n++;
        if (!inside) {
            const char* p = buf;
            if (startsWith(p, "#interface")) {
                p += strlen("#interface");
                while (*p == ' ' || *p == '\t') p++;
                if (*p == '$') p++;
                if (strncmp(p, name, strlen(name)) == 0
                    && !isalnum((unsigned char)p[strlen(name)]) && p[strlen(name)] != '_') {
                    inside = 1;
                    startLine = n;
                }
            }
        } else if (startsWith(buf, "#endinterface")) {
            fclose(fp);
            iface = Iface_define(name, path, startLine, err);
            if (iface) iface->path = Str_dupn(path, strlen(path));
            return iface;
        } else {
            Iface_textPut(buf, (int)strlen(buf));
        }
    }
    fclose(fp);
    if (inside) {
        snprintf(msg, sizeof(msg), "#endinterface is missing for %s", name);
        err(path, startLine, msg);
    } else {
        snprintf(msg, sizeof(msg), "interface %s is not defined in %s", name, path);
        err(input, line, msg);
    }
    return 0;
}

/* ---- output : the interface ---- */

static void writeParams(FILE* fp, const IMethod* m, int withNames)
{
    int i;
    for (i = 0; i < m->nparams; i++) {
        fprintf(fp, ", %s", m->params[i].type);
        if (withNames) fprintf(fp, " %s", m->params[i].name);
    }
}

static void writeArgs(FILE* fp, const IMethod* m)
{
    int i;
    for (i = 0; i < m->nparams; i++) fprintf(fp, ", %s", m->params[i].name);
}

void Iface_emitHeader(const Iface* iface, FILE* h)
{
    int i;
    fprintf(h, "struct %s_vtable {\n", iface->name);
    for (i = 0; i < iface->nmethods; i++) {
        const IMethod* m = &iface->methods[i];
        fprintf(h, "    %s (*%s)(void* self", m->ret, m->name);
        writeParams(h, m, 1);
        fprintf(h, ");\n");
    }
    fprintf(h, "};\n");
    fprintf(h, "typedef struct { const struct %s_vtable* vt; void* self; } %s;\n", iface->name, iface->name);
    for (i = 0; i < iface->nmethods; i++) {
        const IMethod* m = &iface->methods[i];
        fprintf(h, "%s %s_%s(%s r", m->ret, iface->name, m->name, iface->name);
        writeParams(h, m, 1);
        fprintf(h, ");\n");
    }
}

void Iface_emitSource(const Iface* iface, FILE* s)
{
    int i;
    for (i = 0; i < iface->nmethods; i++) {
        const IMethod* m = &iface->methods[i];
        fprintf(s, "%s %s_%s(%s r", m->ret, iface->name, m->name, iface->name);
        writeParams(s, m, 1);
        fprintf(s, ")\n{\n    %sr.vt->%s(r.self", strcmp(m->ret, "void") == 0 ? "" : "return ", m->name);
        writeArgs(s, m);
        fprintf(s, ");\n}\n");
    }
}

/* ---- output : the class ---- */

static char** protos = 0;
static int protoCount = 0;
static Iface** impl = 0;
static int implCount = 0;

void Iface_classBegin(void)
{
    int i;
    for (i = 0; i < protoCount; i++) free(protos[i]);
    free(protos);
    free(impl);
    protos = 0;
    impl = 0;
    protoCount = 0;
    implCount = 0;
}

void Iface_classMethod(const char* proto)
{
    protos = (char**)realloc(protos, sizeof(char*) * (protoCount + 1));
    if (protos == 0) { fprintf(stderr, "error: out of memory\n"); exit(1); }
    protos[protoCount++] = Str_dupn(proto, strlen(proto));
}

void Iface_classImplements(Iface* iface)
{
    int i;
    for (i = 0; i < implCount; i++) if (impl[i] == iface) return;
    impl = (Iface**)realloc(impl, sizeof(Iface*) * (implCount + 1));
    if (impl == 0) { fprintf(stderr, "error: out of memory\n"); exit(1); }
    impl[implCount++] = iface;
}

int Iface_classImplementsCount(void)
{
    return implCount;
}

void Iface_classFinish(const char* cls, const char* file, int line, FILE* source, FILE* header, IfaceErr err)
{
    int i, j, k;
    size_t prefix = strlen(cls) + 1;
    IMethod* ms = (IMethod*)calloc(protoCount + 1, sizeof(IMethod));
    int nms = 0;
    char msg[MSG_SIZE];

    for (i = 0; i < protoCount; i++) {
        IMethod m;
        if (parseProto(protos[i], &m, msg) == 0 && m.nparams >= 1
            && strncmp(m.name, cls, prefix - 1) == 0 && m.name[prefix - 1] == '_') {
            memmove(m.name, m.name + prefix, strlen(m.name + prefix) + 1);
            ms[nms++] = m;
        }
    }
    for (i = 0; i < implCount; i++) {
        const Iface* ifc = impl[i];
        int ok = 1;
        for (j = 0; j < ifc->nmethods; j++) {
            const IMethod* want = &ifc->methods[j];
            const IMethod* have = 0;
            for (k = 0; k < nms; k++) {
                if (strcmp(ms[k].name, want->name) == 0) { have = &ms[k]; break; }
            }
            if (have == 0) {
                snprintf(msg, sizeof(msg), "class %s does not implement %s::%s", cls, ifc->name, want->name);
                err(file, line, msg);
                ok = 0;
                continue;
            }
            {
                int same = strcmp(have->ret, want->ret) == 0 && have->nparams - 1 == want->nparams;
                for (k = 0; same && k < want->nparams; k++) {
                    same = strcmp(have->params[k + 1].type, want->params[k].type) == 0;
                }
                if (!same) {
                    char a[MSG_SIZE / 4], b[MSG_SIZE / 4];
                    describe(want, 0, a, sizeof(a));
                    describe(have, 1, b, sizeof(b));
                    snprintf(msg, sizeof(msg), "%s::%s differs from %s::%s (expected '%s', found '%s')",
                             cls, want->name, ifc->name, want->name, a, b);
                    err(file, line, msg);
                    ok = 0;
                }
            }
        }
        if (!ok) continue;
        for (j = 0; j < ifc->nmethods; j++) {
            const IMethod* m = &ifc->methods[j];
            fprintf(source, "static %s %s_%s_%s(void* self", m->ret, cls, ifc->name, m->name);
            writeParams(source, m, 1);
            fprintf(source, ")\n{\n    %s%s_%s((%s*)self",
                    strcmp(m->ret, "void") == 0 ? "" : "return ", cls, m->name, cls);
            writeArgs(source, m);
            fprintf(source, ");\n}\n");
        }
        fprintf(source, "static const struct %s_vtable %s_%s_vt = {", ifc->name, cls, ifc->name);
        for (j = 0; j < ifc->nmethods; j++) {
            fprintf(source, "%s %s_%s_%s", j ? "," : "", cls, ifc->name, ifc->methods[j].name);
        }
        fprintf(source, " };\n");
        fprintf(source, "%s %s_as_%s(%s* p)\n{\n    %s r;\n    r.vt = &%s_%s_vt;\n    r.self = p;\n    return r;\n}\n",
                ifc->name, cls, ifc->name, cls, ifc->name, cls, ifc->name);
        fprintf(header, "%s %s_as_%s(%s* p);\n", ifc->name, cls, ifc->name, cls);
    }
    for (i = 0; i < nms; i++) freeMethod(&ms[i]);
    free(ms);
}

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

void Iface_addClass(const char* name)
{
    int i;
    for (i = 0; i < classCount; i++) if (strcmp(classes[i], name) == 0) return;
    classes = (char**)realloc(classes, sizeof(char*) * (classCount + 1));
    if (classes == 0) { fprintf(stderr, "error: out of memory\n"); exit(1); }
    classes[classCount++] = Str_dupn(name, strlen(name));
}

int Iface_isClass(const char* name)
{
    int i;
    for (i = 0; i < classCount; i++) if (strcmp(classes[i], name) == 0) return 1;
    return lookup(name) != 0;
}

void Iface_importClass(const char* name, const char* input)
{
    if (Iface_find(name, input, 0, silent) == 0) Iface_addClass(name);
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
    v->info.isInterface = lookup(cls) != 0;
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
    if (Iface_isClass(cls) && stars <= 1) addVar(t, name, cls, stars);
    else removeVar(t, name);    /* a new declaration hides the old one */
}

void Vars_declareProto(const char* proto, int skipThis)
{
    IMethod m;
    char msg[MSG_SIZE];
    int i;
    if (parseProto(proto, &m, msg)) return;
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
    freeMethod(&m);
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

void Iface_returnType(const char* proto, char* out, size_t size)
{
    IMethod m;
    char msg[MSG_SIZE];
    out[0] = 0;
    if (parseProto(proto, &m, msg)) return;
    snprintf(out, size, "%s", m.ret);
    freeMethod(&m);
}
