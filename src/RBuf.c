#include <stdio.h>
#include <string.h>

#include "common.h"

/**
* @class RBuf
* @brief Ring Buffer
*/

#include "RBuf_define.h"
//Class:RBuf 

static const char* TYPE_CAPTION[] = {
    "T_NON      ",
    "T_NUMBER   ",
    "T_WORD     ",
    "T_OBJECT   ",
    "T_CLASS    ", 
    "T_DOT      ",
    "T_KEYWORD  ",
    "T_METHOD   ",
    "T_PARAMETER",
    "T_ASTERISK ",
    "T_TYPE     ",
    "T_SEMICOLON",
    "T_WSPACE   ",
    "T_COMMA    "};

char* RBufElement_toString(char* tostring, const char* str, const buftype type) {
    
    if ( type < sizeof(TYPE_CAPTION)/sizeof(TYPE_CAPTION[0]) ) {
        strcat(tostring, TYPE_CAPTION[type]);
    } else {
        strcat(tostring, "UNKNOWN    ");
    }
    strcat(tostring, ":");
    strcat(tostring, str);
    return tostring;
}

/**
* @brief initialize
* @memberof RBuf
*/
void RBuf_init(RBuf* this, const boolean isThisPointer, boolean isVerbose){
    this->first = 0;
    this->last = 0;
    this->isThisPointer = isThisPointer;
    this->isVerbose = isVerbose;
}

/**
* @brief enque
* @memberof RBuf
*/
void RBuf_enque(RBuf* this, const char* str, const int len, const buftype type){
    memset(&(this->buf[this->last][0]), 0, MAX_TEXT);
    strcpy( &(this->buf[this->last][0]), str);
    this->type[this->last] = type;
    this->last++;
    if (this->first == this->last) {
        /* buffer over */
        if (this->isVerbose) { printf("error:buffer over.\n"); }
    }
    if (this->last==MAX_ARRAYSIZE) {this->last=0;}
}

/**
* @brief deque
* @memberof RBuf
*/
char* RBuf_deque(RBuf* this, buftype* p_type) {
    char* ret = this->buf[this->first];
    *p_type = this->type[this->first];
    this->first++;
    if (this->first==MAX_ARRAYSIZE) {this->first=0;}
    return ret;
}

/**
* @brief empty check
* @memberof RBuf
*/
boolean RBuf_isEmpty(const RBuf* this){
    return (this->first==this->last?TRUE:FALSE);
}

char* RBuf_allDeque(RBuf* this, char* buf, const char* class_buf_str) {
    buftype type;
    char* str;
    buf[0] = 0;
    while(RBuf_isEmpty(this) == FALSE) {
        str = RBuf_deque(this, &type);
        
        char tostring[MAX_TEXT] = {0};
        if (this->isVerbose) printf("element :%s\n", RBufElement_toString(tostring, str, type) );
        
        switch(type) {
        case T_OBJECT:
            if (this->isThisPointer) {
                strcat(buf, "this->");
            }else{
                strcat(buf, "self->");
            }
            break;
        case T_METHOD:
            strcat(buf, class_buf_str);
            strcat(buf, "_");
            break;
        }
        strcat(buf,  str);
    }
    return buf;
}

boolean RBuf_back_retype(RBuf* this, const buftype find, const buftype replace) {
    if (RBuf_isEmpty(this) ) return FALSE;
    
    int index = this->last;
    while( index != this->first) {
        index--;
        if ( index < 0 ) index=MAX_ARRAYSIZE;
        if ( this->type[index] == T_DOT ) { return FALSE; }
        if ( this->type[index] == find ) {
            this->type[index] = replace;
            return TRUE;
        }
    }
    return FALSE;
}

const char* RBuf_back_getStr(const RBuf* this, const buftype find) {
    if (RBuf_isEmpty(this) ) return 0;
    
    int index = this->last;
    while( index != this->first) {
        index--;
        if ( index < 0 ) index=MAX_ARRAYSIZE;
        if ( this->type[index] == find ) {
            return this->buf[index];
        }
    }
    return 0;
}

void RBuf_trimQue(RBuf* this){
    if (this->first == this->last) return;

    if (this->first < this->last) {
        for (int i = this->first; i<this->last; i++)
        {
            if( this->type[i] == T_WSPACE ) {
                this->first++;
            } else {
                break;
            }
        }
    } else {
        boolean isContinue = TRUE;
        for (int i = this->first; i<MAX_ARRAYSIZE; i++)
        {
            if( this->type[i] == T_WSPACE ) {
                this->first++;
            } else {
                isContinue = FALSE;
                break;
            }
        }
        if (isContinue) {
            for (int i = 0; i<this->last; i++)
            {
                if( this->type[i] == T_WSPACE ) {
                    this->first++;
                } else {
                    break;
                }
            }
        }
    }
    return;
}

boolean RBuf_isWSPACE(RBuf* this){
    if (this->first == this->last) return FALSE;

    if (this->first < this->last) {
        for (int i = this->first; i<this->last; i++)
        {
            if(this->type[i]!=T_WSPACE) return FALSE;
        }
    } else {
        for (int i = this->first; i<MAX_ARRAYSIZE; i++)
        {
            if(this->type[i]!=T_WSPACE) return FALSE;
        }
        for (int i = 0; i<this->last; i++)
        {
            if(this->type[i]!=T_WSPACE) return FALSE;
        }
    }
    return TRUE;
}

/**
* @brief toString
* @param buf A string that represents the current object.
* @return A string that represents the current object.
*/
char* RBuf_toString(const RBuf* this, char* tostring) {
    if (this->first == this->last) return tostring;

    if (this->first < this->last) {
        for (int i = this->first; i<this->last; i++)
        {
            char element_tostring[MAX_ARRAYSIZE] = {0};
            strcat(tostring, RBufElement_toString(element_tostring, this->buf[i], this->type[i]) );
            strcat(tostring, "\n");
        }
    } else {
        for (int i = this->first; i<MAX_ARRAYSIZE; i++)
        {
            char element_tostring[MAX_ARRAYSIZE] = {0};
            strcat(tostring, RBufElement_toString(element_tostring, this->buf[i], this->type[i]) );
            strcat(tostring, "\n");
        }
        for (int i = 0; i<this->last; i++)
        {
            char element_tostring[MAX_ARRAYSIZE] = {0};
            strcat(tostring, RBufElement_toString(element_tostring, this->buf[i], this->type[i]) );
            strcat(tostring, "\n");
        }
    }
    return tostring;
}
