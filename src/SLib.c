#include "common.h"

#include "SLib.h"

char* SLib_chopRet(char* buf){
    unsigned int len;
    for (len=0; len<MAX_TEXT*MAX_ARRAYSIZE; len++) {
        if (0==buf[len]) {
            break;
        }
    }
    if (len==0) return buf;
    if ( '\n' == buf[len-1] ) buf[len-1]=0;
    return buf;
}

