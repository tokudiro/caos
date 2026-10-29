#ifndef TEST_OPTION_01_H__
#define TEST_OPTION_01_H__
typedef struct TEST_OPTION_01_struct TEST_OPTION_01;
int TEST_OPTION_01_get_c(const TEST_OPTION_01* pthis);
void TEST_OPTION_01_set_c(TEST_OPTION_01* pthis, int c);
int c; // accessor with a line comment
int TEST_OPTION_01_sum(TEST_OPTION_01* pthis);
int TEST_OPTION_01_calc(TEST_OPTION_01* pthis);
#endif /* TEST_OPTION_01_H__ */
