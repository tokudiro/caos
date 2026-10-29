#ifndef STR_H__
#define STR_H__
/* Small string helpers of the translator (C, not caos). Memory is malloc'ed; out of memory exits. */
#include <stddef.h>

char* Str_dupn(const char* s, size_t n);
char* Str_alloc(size_t n);     /* n + 1 zeroed bytes */
/* trim the blanks at both ends, collapse the others, drop the blanks around '*' */
char* Str_normalize(const char* s, size_t n);
int Str_startsWith(const char* s, const char* prefix);
int Str_isWordChar(int c);     /* a letter, a digit or '_' */
/* append str to buf (length *len, capacity cap including the end mark) ; 0 if it does not fit */
int Str_append(char* buf, size_t* len, size_t cap, const char* str);

#endif /* STR_H__ */
