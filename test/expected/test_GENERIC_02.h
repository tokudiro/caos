#ifndef TEST_GENERIC_02_H__
#define TEST_GENERIC_02_H__
typedef struct Counter_struct Counter;
void Counter_inc(Counter* pthis);
int Counter_get(Counter* pthis);
typedef struct TwiceCounter_struct TwiceCounter;
int TwiceCounter_run(TwiceCounter* pthis, Counter* x);
#endif /* TEST_GENERIC_02_H__ */
