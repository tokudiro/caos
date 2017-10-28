#ifndef BUFTYPE_H__
#define BUFTYPE_H__
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
    T_POINTER,
	T_TYPE,
    T_SEMICOLON,
	T_WSPACE,
	T_COMMA} buftype;
#endif /* BUFTYPE_H__ */
