#ifndef TEST_GENERIC_01_H__
#define TEST_GENERIC_01_H__
typedef struct Counter Counter;
void Counter_inc(Counter* pthis);
typedef struct IntChar IntChar;
int IntChar_getFirst(IntChar* pthis);
void IntChar_set(IntChar* pthis, int a, char b);
typedef struct CounterPtr CounterPtr;
Counter* CounterPtr_getFirst(CounterPtr* pthis);
void CounterPtr_set(CounterPtr* pthis, Counter* a, int b);
struct Counter{
int n;
};
struct IntChar{
int first;
char second;
};
struct CounterPtr{
Counter* first;
int second;
};
#endif /* TEST_GENERIC_01_H__ */
