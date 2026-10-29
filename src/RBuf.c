#include <stdio.h>
#include <string.h>

#include "common.h"

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
    "T_POINTER  ",
    "T_TYPE     ",
    "T_VALUE    ",
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
* @class RBuf
* @brief Ring Buffer
*/
#include "RBuf.h"
#include "RBuf_define.h"




/**
* @brief initialize
* @memberof RBuf
*/
void RBuf_init(RBuf* pthis, const char* thispointer_str, boolean isVerbose){
    pthis->first = 0;
    pthis->last = 0;
    pthis->overflow = FALSE;
    pthis->thispointer_str[0] = 0;
    strncat( pthis->thispointer_str, thispointer_str, MAX_TEXT - 1);
    pthis->isVerbose = isVerbose;
}

/**
* @brief set class string
* @memberof RBuf
*/
void RBuf_setClass(RBuf* pthis, const char* thisclass_str){
    pthis->thisclass_str[0] = 0;
    strncat( pthis->thisclass_str, thisclass_str, MAX_TEXT - 1);
}

/**
* @brief enque
* @memberof RBuf
*/
void RBuf_enque(RBuf* pthis, const char* str, const int len, const buftype type){
    (void)len;  /* kept for the interface : the whole string is copied */
    int next = (pthis->last + 1 == MAX_ARRAYSIZE) ? 0 : pthis->last + 1;
    if (next == pthis->first) {
        /* the buffer is full : keep what is queued and report it */
        pthis->overflow = TRUE;
        return;
    }
    memset(&(pthis->buf[pthis->last][0]), 0, MAX_TEXT);
    strncpy( &(pthis->buf[pthis->last][0]), str, MAX_TEXT - 1);
    pthis->type[pthis->last] = type;
    pthis->last = next;
}

/**
* @brief true (once) if enque had to drop an element because the buffer was full
* @memberof RBuf
*/
boolean RBuf_popOverflow(RBuf* pthis){
    boolean ret = pthis->overflow;
    pthis->overflow = FALSE;
    return ret;
}

/**
* @brief deque
* @memberof RBuf
*/
char* RBuf_deque(RBuf* pthis, buftype* p_type){
    char* ret = pthis->buf[pthis->first];
    *p_type = pthis->type[pthis->first];
    pthis->first++;
    if (pthis->first==MAX_ARRAYSIZE) {pthis->first=0;}
    return ret;
}

/**
* @brief empty check
* @memberof RBuf
*/
boolean RBuf_isEmpty(const RBuf* pthis){
    return (pthis->first==pthis->last?TRUE:FALSE);
}

char* RBuf_allDeque(RBuf* pthis, char* buf){
    buftype type;
    char* str;
    buf[0] = 0;
    while(RBuf_isEmpty(pthis) == FALSE) {
        str = RBuf_deque(pthis, &type);
        
        char tostring[MAX_TEXT*2] = {0};
        if (pthis->isVerbose) printf("element :%s\n", RBufElement_toString(tostring, str, type) );
        
        switch(type) {
        case T_OBJECT:
            strcat(buf, pthis->thispointer_str);
            strcat(buf, "->");
            break;
        case T_METHOD:
            strcat(buf, pthis->thisclass_str);
            strcat(buf, "_");
            break;
        default:
            break;
        }
        strcat(buf,  str);
    }
    if (pthis->isVerbose) printf("----------\n");
    return buf;
}

boolean RBuf_back_retype(RBuf* pthis, const buftype find, const buftype replace){
    return RBuf_back_retype_stop(pthis, find, replace, T_DOT);
}

/**
* @brief retype the nearest element of type find before the end,
*        giving up at T_DOT or at the type stop
* @memberof RBuf
*/
boolean RBuf_back_retype_stop(RBuf* pthis, const buftype find, const buftype replace, const buftype stop){
    if (RBuf_isEmpty(pthis) ) return FALSE;
    
    int index = pthis->last;
    while( index != pthis->first) {
        index--;
        if ( index < 0 ) index=MAX_ARRAYSIZE-1;
        if ( pthis->type[index] == T_DOT || pthis->type[index] == stop ) { return FALSE; }
        if ( pthis->type[index] == find ) {
            pthis->type[index] = replace;
            return TRUE;
        }
    }
    return FALSE;
}

/**
* @brief copy the text before the T_PARAMETER element (the declared type)
*        out holds MAX_TEXT chars : a longer type is cut
* @return out, or 0 if there is no T_PARAMETER element
* @memberof RBuf
*/
char* RBuf_typeBeforeParameter(const RBuf* pthis, char* out){
    int i = pthis->first;
    int len;
    out[0] = 0;
    while (i != pthis->last) {
        if (pthis->type[i] == T_PARAMETER) {
            len = strlen(out);
            while (len > 0 && out[len-1] == ' ') {
                out[--len] = 0;
            }
            return out;
        }
        strncat(out, pthis->buf[i], MAX_TEXT - 1 - strlen(out));
        i++;
        if (i == MAX_ARRAYSIZE) i = 0;
    }
    return 0;
}

const char* RBuf_back_getStr(const RBuf* pthis, const buftype find){
    if (RBuf_isEmpty(pthis) ) return 0;
    
    int index = pthis->last;
    while( index != pthis->first) {
        index--;
        if ( index < 0 ) index=MAX_ARRAYSIZE-1;
        if ( pthis->type[index] == find ) {
            return pthis->buf[index];
        }
    }
    return 0;
}

void RBuf_trimQue(RBuf* pthis){
    int i;
    if (pthis->first == pthis->last) return;

    if (pthis->first < pthis->last) {
        for (i = pthis->first; i<pthis->last; i++)
        {
            if( pthis->type[i] == T_WSPACE ) {
                pthis->first++;
            } else {
                break;
            }
        }
    } else {
        boolean isContinue = TRUE;
        for (i = pthis->first; i<MAX_ARRAYSIZE; i++)
        {
            if( pthis->type[i] == T_WSPACE ) {
                pthis->first++;
            } else {
                isContinue = FALSE;
                break;
            }
        }
        if (isContinue) {
            for (i = 0; i<pthis->last; i++)
            {
                if( pthis->type[i] == T_WSPACE ) {
                    pthis->first++;
                } else {
                    break;
                }
            }
        }
    }
    return;
}

boolean RBuf_isWSPACE(RBuf* pthis){
    int i;
    if (pthis->first == pthis->last) return FALSE;

    if (pthis->first < pthis->last) {
        for (i = pthis->first; i<pthis->last; i++)
        {
            if(pthis->type[i]!=T_WSPACE) return FALSE;
        }
    } else {
        for (i = pthis->first; i<MAX_ARRAYSIZE; i++)
        {
            if(pthis->type[i]!=T_WSPACE) return FALSE;
        }
        for (i = 0; i<pthis->last; i++)
        {
            if(pthis->type[i]!=T_WSPACE) return FALSE;
        }
    }
    return TRUE;
}

/**
* @brief toString
* @param buf A string that represents the current object.
* @return A string that represents the current object.
*/
char* RBuf_toString(const RBuf* pthis, char* tostring){
    int i;
    if (pthis->first == pthis->last) return tostring;

    if (pthis->first < pthis->last) {
        for (i = pthis->first; i<pthis->last; i++)
        {
            char element_tostring[MAX_TEXT*2] = {0};
            strcat(tostring, RBufElement_toString(element_tostring, pthis->buf[i], pthis->type[i]) );
            strcat(tostring, "\n");
        }
    } else {
        for (i = pthis->first; i<MAX_ARRAYSIZE; i++)
        {
            char element_tostring[MAX_TEXT*2] = {0};
            strcat(tostring, RBufElement_toString(element_tostring, pthis->buf[i], pthis->type[i]) );
            strcat(tostring, "\n");
        }
        for (i = 0; i<pthis->last; i++)
        {
            char element_tostring[MAX_TEXT*2] = {0};
            strcat(tostring, RBufElement_toString(element_tostring, pthis->buf[i], pthis->type[i]) );
            strcat(tostring, "\n");
        }
    }
    return tostring;
}


