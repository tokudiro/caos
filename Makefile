.PHONY: all
all : caos

.PHONY: clean
clean :
	$(RM) caos ./obj/caos.l ./obj/caos.yy.c

./obj/caos.l : ./src/*.l
	@if [ ! -d obj ]; then \
		mkdir obj; \
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

./obj/caos.yy.c : ./obj/caos.l
	$(LEX) -o./obj/caos.yy.c ./obj/caos.l

caos : ./obj/caos.yy.c ./src/RBuf.c ./src/main.c ./src/SBuf.c ./src/SLib.c -lfl
	$(CC) -o caos ./obj/caos.yy.c ./src/RBuf.c ./src/main.c ./src/SBuf.c ./src/SLib.c -L/usr/lib -lfl -I./src

.PHONY: test
test :
	cd test
	$(RM) test_*.c
	$(RM) test_*.h
	../caos -L test_METHOD_SCOPE_01.caos
	$(CC) test_METHOD_SCOPE_01.c -o test_METHOD_SCOPE_01
	../caos -L test_METHOD_DEFINE_01.caos
	$(CC) test_METHOD_DEFINE_01.c -o test_METHOD_DEFINE_01
	./test_METHOD_DEFINE_01
	../caos -L test_METHOD_DEFINE_02.caos
	$(CC) test_METHOD_DEFINE_02.c -o test_METHOD_DEFINE_02
	./test_METHOD_DEFINE_02
	../caos -Lp test_TYPE_STRUCT.caos
	$(CC) test_TYPE_STRUCT.c -o test_TYPE_STRUCT
	./test_TYPE_STRUCT
	../caos -Ldp test_TYPE_UTILITY.caos
	$(CC) test_TYPE_UTILITY.c -o test_TYPE_UTILITY
	./test_TYPE_UTILITY
	../caos -L test_INITIAL_01.caos
	$(CC) test_INITIAL_01.c -o test_INITIAL_01
	cd ..

.PHONY: clean-test
clean-test :
	cd test
	$(RM) test_*.c
	$(RM) test_*.h
	cd ..
