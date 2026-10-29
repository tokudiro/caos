#ifndef COUNTER_H__
#define COUNTER_H__
typedef struct Counter_struct Counter;
void Counter_add(Counter* pthis, int d);
int Counter_get(Counter* pthis);
#endif /* COUNTER_H__ */
