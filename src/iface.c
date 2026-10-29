#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#include "iface.h"
#include "proto.h"
#include "str.h"

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

Iface* Iface_lookup(const char* name)
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
    char* body = Proto_clean(text ? text : "");
    char* s = body;
    char msg[PROTO_MSG_SIZE];
    int i, j;

    if (Iface_lookup(name)) {
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
            decl = Str_normalize(s, strlen(s));
            if (decl[0]) err(file, line, "';' is missing in the interface");
            free(decl);
            break;
        }
        decl = Str_normalize(s, semi - s);
        s = semi + 1;
        if (decl[0] == 0) { free(decl); continue; }
        if (Proto_parse(decl, &m, msg)) {
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

Iface* Iface_find(const char* name, const char* input, int line, IfaceErr err)
{
    Iface* iface = Iface_lookup(name);
    const char* slash;
    char path[4096];
    char buf[1024];
    char msg[PROTO_MSG_SIZE];
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
            if (Str_startsWith(p, "#interface")) {
                p += strlen("#interface");
                while (*p == ' ' || *p == '\t') p++;
                if (*p == '$') p++;
                if (strncmp(p, name, strlen(name)) == 0
                    && !isalnum((unsigned char)p[strlen(name)]) && p[strlen(name)] != '_') {
                    inside = 1;
                    startLine = n;
                }
            }
        } else if (Str_startsWith(buf, "#endinterface")) {
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
    char msg[PROTO_MSG_SIZE];

    for (i = 0; i < protoCount; i++) {
        IMethod m;
        if (Proto_parse(protos[i], &m, msg) == 0 && m.nparams >= 1
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
                    char a[PROTO_MSG_SIZE / 4], b[PROTO_MSG_SIZE / 4];
                    Proto_describe(want, 0, a, sizeof(a));
                    Proto_describe(have, 1, b, sizeof(b));
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
    for (i = 0; i < nms; i++) Proto_free(&ms[i]);
    free(ms);
}
