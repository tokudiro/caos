#ifndef TEST_ATTRIBUTE_01_H__
#define TEST_ATTRIBUTE_01_H__
typedef struct TEST_ATTRIBUTE_01_struct TEST_ATTRIBUTE_01;
int TEST_ATTRIBUTE_01_get_i(const TEST_ATTRIBUTE_01* pthis);
void TEST_ATTRIBUTE_01_set_i(TEST_ATTRIBUTE_01* pthis, int i);
unsigned int TEST_ATTRIBUTE_01_get_u(const TEST_ATTRIBUTE_01* pthis);
void TEST_ATTRIBUTE_01_set_u(TEST_ATTRIBUTE_01* pthis, unsigned int u);
uint32_t TEST_ATTRIBUTE_01_get_w(const TEST_ATTRIBUTE_01* pthis);
void TEST_ATTRIBUTE_01_set_w(TEST_ATTRIBUTE_01* pthis, uint32_t w);
char* TEST_ATTRIBUTE_01_get_p(const TEST_ATTRIBUTE_01* pthis);
void TEST_ATTRIBUTE_01_set_p(TEST_ATTRIBUTE_01* pthis, char* p);
const char* TEST_ATTRIBUTE_01_get_name(const TEST_ATTRIBUTE_01* pthis);
void TEST_ATTRIBUTE_01_set_name(TEST_ATTRIBUTE_01* pthis, const char* name);
struct TEST_ATTRIBUTE_01_struct* TEST_ATTRIBUTE_01_get_self(const TEST_ATTRIBUTE_01* pthis);
void TEST_ATTRIBUTE_01_set_self(TEST_ATTRIBUTE_01* pthis, struct TEST_ATTRIBUTE_01_struct* self);
const int TEST_ATTRIBUTE_01_get_fixed(const TEST_ATTRIBUTE_01* pthis);
int TEST_ATTRIBUTE_01_sum(TEST_ATTRIBUTE_01* pthis);
#endif /* TEST_ATTRIBUTE_01_H__ */
