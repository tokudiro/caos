#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#include "str.h"
#include "template.h"

#define MSG_SIZE 5120

static void say(IfaceErr err, const char* file, int line, const char* fmt, const char* a, const char* b)
{
    char msg[MSG_SIZE];
    snprintf(msg, sizeof(msg), fmt, a, b);
    err(file, line, msg);
}

/* ---- the text of a template ---- */

static char* text = 0;
static size_t textLen = 0;

void Template_textBegin(void)
{
    free(text);
    text = Str_alloc(0);
    textLen = 0;
}

void Template_textPut(const char* str, int len)
{
    char* p = (char*)realloc(text, textLen + len + 1);
    if (p == 0) { fprintf(stderr, "error: out of memory\n"); exit(1); }
    text = p;
    memcpy(text + textLen, str, len);
    textLen += len;
    text[textLen] = 0;
}

static Template** registry = 0;
static int registryCount = 0;

static Template* lookup(const char* name)
{
    int i;
    for (i = 0; i < registryCount; i++) {
        if (strcmp(registry[i]->name, name) == 0) return registry[i];
    }
    return 0;
}

/* "<T, U> tail" -> the parameter names and the tail ; returns 0 if the head is malformed */
static int parseHead(const char* head, Template* t, const char** reason)
{
    const char* p = head;
    const char* gt;
    while (*p == ' ' || *p == '\t') p++;
    if (*p != '<') { *reason = "'<' is missing"; return 0; }
    gt = strchr(p, '>');
    if (gt == 0) { *reason = "'>' is missing"; return 0; }
    p++;
    while (p < gt) {
        const char* b;
        int i;
        while (p < gt && (*p == ' ' || *p == '\t' || *p == ',')) p++;
        if (p >= gt) break;
        if (!(isalpha((unsigned char)*p) || *p == '_')) { *reason = "a type parameter name is expected"; return 0; }
        b = p;
        while (p < gt && Str_isWordChar(*p)) p++;
        for (i = 0; i < t->nparams; i++) {
            if (strlen(t->params[i]) == (size_t)(p - b) && strncmp(t->params[i], b, p - b) == 0) {
                *reason = "a type parameter is declared twice";
                return 0;
            }
        }
        t->params = (char**)realloc(t->params, sizeof(char*) * (t->nparams + 1));
        if (t->params == 0) { fprintf(stderr, "error: out of memory\n"); exit(1); }
        t->params[t->nparams++] = Str_dupn(b, p - b);
    }
    if (t->nparams == 0) { *reason = "no type parameter"; return 0; }
    t->tail = Str_dupn(gt + 1, strlen(gt + 1));
    return 1;
}

Template* Template_define(const char* name, const char* head, const char* directive,
                          const char* endDirective, int line, const char* file, IfaceErr err)
{
    Template* t;
    const char* reason = "";
    if (lookup(name)) {
        say(err, file, line, "template %s is defined twice", name, "");
        return 0;
    }
    t = (Template*)calloc(1, sizeof(Template));
    if (t == 0) { fprintf(stderr, "error: out of memory\n"); exit(1); }
    t->name = Str_dupn(name, strlen(name));
    if (!parseHead(head, t, &reason)) {
        say(err, file, line, "template %s: %s", name, reason);
        return 0;
    }
    t->directive = Str_dupn(directive, strlen(directive));
    t->endDirective = Str_dupn(endDirective, strlen(endDirective));
    t->body = Str_dupn(text ? text : "", text ? strlen(text) : 0);
    t->line = line;
    registry = (Template**)realloc(registry, sizeof(Template*) * (registryCount + 1));
    if (registry == 0) { fprintf(stderr, "error: out of memory\n"); exit(1); }
    registry[registryCount++] = t;
    return t;
}

/* ---- reading another file ---- */

static Template* find(const char* name, const char* input, int line, IfaceErr err)
{
    static const char* opening[] = { "#class", "#singleton", "#struct", "#utility", 0 };
    static const char* closing[] = { "#endclass", "#endsingleton", "#endstruct", "#endutility", 0 };
    Template* t = lookup(name);
    const char* slash;
    char path[4096];
    char buf[1024];
    FILE* fp;
    int inside = 0, startLine = 0, n = 0, kind = 0, i;
    char head[1024];

    if (t) return t;
    slash = strrchr(input, '/');
    snprintf(path, sizeof(path), "%.*s%s.caos", slash ? (int)(slash - input + 1) : 0, input, name);
    fp = fopen(path, "r");
    if (fp == 0) {
        say(err, input, line, "template %s is not defined (define it before #instance, or in a file named after it)", name, "");
        return 0;
    }
    Template_textBegin();
    while (fgets(buf, sizeof(buf), fp)) {
        n++;
        if (!inside) {
            for (i = 0; opening[i]; i++) {
                const char* p;
                size_t l = strlen(name);
                if (!Str_startsWith(buf, opening[i])) continue;
                p = buf + strlen(opening[i]);
                while (*p == ' ' || *p == '\t') p++;
                if (*p == '$') p++;
                if (strncmp(p, name, l) == 0 && p[l] == '<') {
                    inside = 1;
                    startLine = n;
                    kind = i;
                    snprintf(head, sizeof(head), "%s", p + l);
                    head[strcspn(head, "\r\n")] = 0;
                }
            }
        } else if (Str_startsWith(buf, closing[kind])) {
            fclose(fp);
            t = Template_define(name, head, opening[kind], closing[kind], startLine, path, err);
            if (t) t->path = Str_dupn(path, strlen(path));
            return t;
        } else {
            Template_textPut(buf, (int)strlen(buf));
        }
    }
    fclose(fp);
    if (inside) say(err, path, startLine, "%s is not closed (%s is missing)", name, closing[kind]);
    else say(err, input, line, "template %s is not defined in %s", name, path);
    return 0;
}

/* ---- the expansion ---- */

/* "$Name" -> "Name*" in a type argument ; the result is malloc'ed */
static char* typeArgument(const char* s, size_t n)
{
    char* out = Str_alloc(n * 2 + 2);
    size_t i = 0, o = 0;
    while (i < n && isspace((unsigned char)s[i])) i++;
    while (n > i && isspace((unsigned char)s[n-1])) n--;
    while (i < n) {
        if (s[i] == '$' && i + 1 < n && (isalpha((unsigned char)s[i+1]) || s[i+1] == '_')) {
            i++;
            while (i < n && Str_isWordChar(s[i])) out[o++] = s[i++];
            out[o++] = '*';
        } else {
            out[o++] = s[i++];
        }
    }
    out[o] = 0;
    return out;
}

/* a type argument that is a plain name (can be joined with "_method") */
static int isPlainName(const char* s)
{
    if (!(isalpha((unsigned char)*s) || *s == '_')) return 0;
    for (; *s; s++) if (!Str_isWordChar(*s)) return 0;
    return 1;
}

/*
 * The body with the type parameters and "$Name" replaced. A word "T_name"
 * becomes "Arg_name" (a call of the method of the type argument); *bad gets
 * the type argument that cannot be used so, and 0 is returned.
 */
static char* substitute(const Template* t, const char* inst, char** args, const char** bad, char* badWord)
{
    size_t n = strlen(t->body), cap = n + 1, o = 0, i = 0;
    char* out = Str_alloc(cap);
    while (i < n) {
        size_t j, len;
        const char* rep = 0;
        size_t replen = 0;
        char joined[512];
        if (!(isalpha((unsigned char)t->body[i]) || t->body[i] == '_') || (i > 0 && Str_isWordChar(t->body[i-1]))) {
            if (o + 2 > cap) { cap = cap * 2 + 2; out = (char*)realloc(out, cap); }
            if (out == 0) { fprintf(stderr, "error: out of memory\n"); exit(1); }
            out[o++] = t->body[i++];
            continue;
        }
        j = i;
        while (j < n && Str_isWordChar(t->body[j])) j++;
        len = j - i;
        if (i > 0 && t->body[i-1] == '$') {
            if (strlen(t->name) == len && strncmp(t->name, t->body + i, len) == 0) {
                rep = inst;
                replen = strlen(inst);
            }
        } else {
            int k;
            for (k = 0; k < t->nparams; k++) {
                size_t pl = strlen(t->params[k]);
                if (pl == len && strncmp(t->params[k], t->body + i, len) == 0) {
                    rep = args[k];
                    replen = strlen(args[k]);
                } else if (len > pl + 1 && strncmp(t->params[k], t->body + i, pl) == 0 && t->body[i + pl] == '_') {
                    if (!isPlainName(args[k])) {
                        *bad = args[k];
                        snprintf(badWord, 256, "%.*s", (int)(len < 255 ? len : 255), t->body + i);
                        free(out);
                        return 0;
                    }
                    snprintf(joined, sizeof(joined), "%s%.*s", args[k], (int)(len - pl), t->body + i + pl);
                    rep = joined;
                    replen = strlen(joined);
                }
            }
        }
        if (rep == 0) { rep = t->body + i; replen = len; }
        if (o + replen + 1 > cap) { cap = cap * 2 + replen + 1; out = (char*)realloc(out, cap); }
        if (out == 0) { fprintf(stderr, "error: out of memory\n"); exit(1); }
        memcpy(out + o, rep, replen);
        o += replen;
        i = j;
    }
    out[o] = 0;
    return out;
}

static const char* skip(const char* p)
{
    while (*p == ' ' || *p == '\t') p++;
    return p;
}

/* an optional '$' and a word */
static const char* readWord(const char* p, char* dst, size_t size)
{
    size_t l = 0;
    if (*p == '$') p++;
    while (Str_isWordChar(*p) && l < size - 1) dst[l++] = *p++;
    dst[l] = 0;
    return p;
}

char* Template_instance(const char* s, const char* input, int lineNo, IfaceErr err,
                        int* line, const char** path)
{
    char inst[256], tname[256];
    const char* p = s;
    const char* gt;
    const char* argBeg;
    char** args = 0;
    int nargs = 0, i;
    Template* t;
    char* body;
    char* out;
    size_t size;

    p = skip(p);
    p = readWord(p, inst, sizeof(inst));
    p = skip(p);
    if (inst[0] == 0 || *p != '=') {
        say(err, input, lineNo, "#instance: expected '$Name = $Template<types>'%s%s", "", "");
        return 0;
    }
    p++;
    p = skip(p);
    p = readWord(p, tname, sizeof(tname));
    p = skip(p);
    if (tname[0] == 0 || *p != '<' || (gt = strrchr(p, '>')) == 0) {
        say(err, input, lineNo, "#instance: expected '$Name = $Template<types>'%s%s", "", "");
        return 0;
    }
    for (argBeg = gt + 1; *argBeg; argBeg++) {
        if (!isspace((unsigned char)*argBeg)) {
            say(err, input, lineNo, "#instance: unexpected text after '>'%s%s", "", "");
            return 0;
        }
    }
    t = find(tname, input, lineNo, err);
    if (t == 0) return 0;

    /* the type arguments (a nested <...> is not supported) */
    argBeg = p + 1;
    for (p = argBeg; p <= gt; p++) {
        if (p < gt && *p == '<') {
            say(err, input, lineNo, "#instance: a type argument cannot contain '<'%s%s", "", "");
            return 0;
        }
        if (p == gt || *p == ',') {
            args = (char**)realloc(args, sizeof(char*) * (nargs + 1));
            if (args == 0) { fprintf(stderr, "error: out of memory\n"); exit(1); }
            args[nargs] = typeArgument(argBeg, p - argBeg);
            if (args[nargs][0] == 0) {
                say(err, input, lineNo, "#instance: a type argument is empty%s%s", "", "");
                return 0;
            }
            nargs++;
            argBeg = p + 1;
        }
    }
    if (nargs != t->nparams) {
        char want[16];
        snprintf(want, sizeof(want), "%d", t->nparams);
        say(err, input, lineNo, "template %s takes %s type argument(s)", t->name, want);
        return 0;
    }
    {
        const char* bad = 0;
        char badWord[256];
        body = substitute(t, inst, args, &bad, badWord);
        if (body == 0) {
            char msg[MSG_SIZE];
            snprintf(msg, sizeof(msg), "#instance: the type argument '%s' cannot be used in '%s' (it must be a plain name)", bad, badWord);
            err(input, lineNo, msg);
            return 0;
        }
    }
    size = strlen(t->directive) + strlen(inst) + strlen(t->tail) + strlen(body) + strlen(t->endDirective) + 16;
    out = Str_alloc(size);
    snprintf(out, size + 1, "%s $%s%s\n%s%s\n", t->directive, inst, t->tail, body, t->endDirective);
    for (i = 0; i < nargs; i++) free(args[i]);
    free(args);
    free(body);
    *line = t->line;
    *path = t->path;
    return out;
}
