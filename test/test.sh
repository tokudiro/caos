#!/bin/sh
rm test_*.c
rm test_*.h

../caos -L test_METHOD_SCOPE_01.caos
gcc test_METHOD_SCOPE_01.c -o test_METHOD_SCOPE_01
../caos -L test_METHOD_DEFINE_01.caos
gcc test_METHOD_DEFINE_01.c -o test_METHOD_DEFINE_01
./test_METHOD_DEFINE_01

../caos -L test_METHOD_DEFINE_02.caos
gcc test_METHOD_DEFINE_02.c -o test_METHOD_DEFINE_02
./test_METHOD_DEFINE_02

../caos -Lp test_TYPE_STRUCT.caos
gcc test_TYPE_STRUCT.c -o test_TYPE_STRUCT
./test_TYPE_STRUCT

../caos -Ldp test_TYPE_UTILITY.caos
gcc test_TYPE_UTILITY.c -o test_TYPE_UTILITY
./test_TYPE_UTILITY

../caos -L test_INITIAL_01.caos
gcc test_INITIAL_01.c -o test_INITIAL_01
