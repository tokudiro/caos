cd ..
./build.sh
cd test
../caos -L test_METHOD_SCOPE_01.caos
gcc test_METHOD_SCOPE_01.c -o test_METHOD_SCOPE_01.exe

../caos -L test_METHOD_DEFINE_01.caos
gcc test_METHOD_DEFINE_01.c -o test_METHOD_DEFINE_01.exe
./test_METHOD_DEFINE_01

../caos -L test_METHOD_DEFINE_02.caos
gcc test_METHOD_DEFINE_02.c -o test_METHOD_DEFINE_02.exe
./test_METHOD_DEFINE_02
