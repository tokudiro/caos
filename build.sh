#!/bin/bash

if [ ! -e obj ]; then
    mkdir obj
fi

echo %{>  ./obj/caos.l
cat ./src/caos.c    >> ./obj/caos.l
echo %}>> ./obj/caos.l
cat ./src/keyword.l >> ./obj/caos.l
echo %%>> ./obj/caos.l
cat ./src/INITIAL.l \
    ./src/IMPORT.l \
    ./src/CLASS_DEFINE.l \
    ./src/CLASS_SCOPE.l \
    ./src/METHOD_DEFINE.l \
    ./src/METHOD_SCOPE.l \
    ./src/ATTRIBUTE_DEFINE.l \
    ./src/EOF.l \
    >> ./obj/caos.l
flex -o./obj/caos.yy.c ./obj/caos.l
gcc -o caos ./obj/caos.yy.c ./src/RBuf.c ./src/main.c ./src/SBuf.c -L/usr/lib -lfl -I./src
