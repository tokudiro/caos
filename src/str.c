#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "str.h"

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
char* Str_normalize(const char* s, size_t n)
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

int Str_startsWith(const char* s, const char* prefix)
{
    return strncmp(s, prefix, strlen(prefix)) == 0;
}

int Str_isWordChar(int c)
{
    return isalnum((unsigned char)c) || c == '_';
}

int Str_append(char* buf, size_t* len, size_t cap, const char* str)
{
    size_t n = strlen(str);
    if (*len + n >= cap) return 0;
    memcpy(buf + *len, str, n);
    *len += n;
    buf[*len] = 0;
    return 1;
}
