.PHONY: all
all : caos

.PHONY: clean
clean :
	$(RM) caos ./obj/caos.l ./obj/caos.yy.c

./obj/caos.l : ./src/*.l
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

./obj/caos.yy.c : ./obj/caos.l
	$(LEX) -o./obj/caos.yy.c ./obj/caos.l

caos : ./obj/caos.yy.c ./src/RBuf.c ./src/main.c ./src/SBuf.c ./src/SLib.c -lfl
	$(CC) -o caos ./obj/caos.yy.c ./src/RBuf.c ./src/main.c ./src/SBuf.c ./src/SLib.c -L/usr/lib -lfl -I./src

