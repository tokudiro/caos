#ifndef TEST_CONTRACT_01_H__
#define TEST_CONTRACT_01_H__
typedef struct Stack_struct Stack;
void Stack_init(Stack* pthis);
void Stack_push(Stack* pthis, int v)


;
int Stack_pop(Stack* pthis)

;
int Stack_size(Stack* pthis);
int Stack_dbl(Stack* pthis, int v);
int Stack_twice(int v)

;
void Stack_check(Stack* pthis)
;
#endif /* TEST_CONTRACT_01_H__ */
