#include <stdio.h>
#include <string.h>

#include "boolean.h"
#include "buftype.h"
#include "common.h"

#include "TestBuf.h"
#include "TestBuf_define.h"
#include "TestBuf_private.h"

int main(void)
{
	struct TestBuffer_struct buf;
	char longbuf[100] = {0};
	TestBuffer_init(&buf);
	TestBuffer_enque(&buf, "aaa", 3, T_WORD);
	TestBuffer_enque(&buf, "bb", 2, T_WORD);
	printf("%s\n", TestBuffer_allque(&buf, longbuf, "zzz") );
	printf("%d\n", TestBuffer_private(&buf) );
	return 0;
}

