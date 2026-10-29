#ifndef TEST_INVARIANT_01_H__
#define TEST_INVARIANT_01_H__
typedef struct Range_struct Range;
void Range_init(Range* pthis, int low, int high)
;
void Range_widen(Range* pthis, int d);
int Range_width(Range* pthis)
;
int Range_peekLow(const Range* pthis);
void Range_breakIt(Range* pthis);
int Range_make(int v);
#endif /* TEST_INVARIANT_01_H__ */
