#ifndef __BUFTYPE__H__
#define __BUFTYPE__H__
typedef enum _buftype { 
	T_NON, 
	T_NUMBER, 
	T_WORD, 
	T_OBJECT, 
	T_CLASS, 
	T_DOT, 
	T_KEYWORD, 
	T_METHOD, 
	T_PARAMETER,
	T_ASTERISK,
	T_TYPE,
    T_SEMICOLON} buftype;
#endif /* __BUFTYPE__H__ */
