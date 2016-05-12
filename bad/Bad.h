#ifndef __Bad_H__
#define __Bad_H__
/* class Bad */
typedef struct Bad_struct Bad;
Bad* Bad_getInstance();
 ;
int Bad_get_z(const Bad* this);
A* Bad_get_a(const Bad* this);
#endif /* __Bad_H__ */
