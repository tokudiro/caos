#!/bin/sh
# usage: test.sh           translate, compile, run, and compare against expected/
#        UPDATE=1 test.sh  regenerate expected/ from the current output
# (run from any directory; requires ../caos to be built)
cd "$(dirname "$0")" || exit 1

rm -f test_*.c test_*.h

failed=0
mkdir -p expected

# check_output <test name>: compare generated .c/.h files with expected/
check_output() {
    name=test_$1
    if [ -n "$UPDATE" ]; then
        rm -f expected/"$name".* expected/"$name"_*
        cp "$name".c "$name".h "$name"_*.h expected/ 2>/dev/null
        return 0
    fi
    ok=0
    for f in "$name".c "$name".h "$name"_*.h expected/"$name".* expected/"$name"_*; do
        [ -e "$f" ] || continue
        g=${f#expected/}
        diff -u "expected/$g" "$g" >&2 || ok=1
    done
    return $ok
}

# run_test <caos option> <test name>
run_test() {
    opt=$1
    name=test_$2
    if ../caos "$opt" "$name.caos" \
        && check_output "$2" \
        && gcc "$name.c" -o "$name" \
        && "./$name"; then
        echo "PASS: $name"
    else
        echo "FAIL: $name"
        failed=1
    fi
}

run_test -L   METHOD_SCOPE_01
run_test -L   METHOD_DEFINE_01
run_test -L   METHOD_DEFINE_02
run_test -Lp  TYPE_STRUCT
run_test -Ldp TYPE_UTILITY
run_test -L   INITIAL_01
run_test -L   COMMENT_01
run_test -L   ATTRIBUTE_01
run_test -L   LONG_01
run_test -L   LONG_02
run_test -L   GENERIC_01
run_test -L   GENERIC_02

# a member initializer is reported (with its line), caos exits non-zero,
# and the output is still valid C without the initializers
if ../caos -L test_ERROR_01.caos 2> out_err.txt; then
    echo "FAIL: member initializer error (caos succeeded)"
    failed=1
elif [ "$(grep -c 'test_ERROR_01.caos:[0-9]*: error: member initializer' out_err.txt)" = 3 ] \
    && grep -q ':3: error' out_err.txt && grep -q ':4: error' out_err.txt && grep -q ':5: error' out_err.txt \
    && gcc test_ERROR_01.c -o test_ERROR_01 && ./test_ERROR_01; then
    echo "PASS: member initializer error"
else
    echo "FAIL: member initializer error"
    cat out_err.txt
    failed=1
fi
rm -f out_err.txt

# malformed inputs : each test/error/*.caos must make caos exit non-zero
# and print exactly the errors listed in the matching .expected file
rm -rf out && mkdir out
for f in error/*.caos; do
    n=${f%.caos}
    if ../caos -L -o out "$f" 2> out/stderr.txt; then
        echo "FAIL: $n (caos succeeded)"
        failed=1
    elif diff -u "$n.expected" out/stderr.txt; then
        echo "PASS: $n"
    else
        echo "FAIL: $n"
        failed=1
    fi
done
rm -rf out

# -o : files go to the given folder with the same content
rm -rf out && mkdir out
if ../caos -L -o out test_INITIAL_01.caos \
    && [ ! -e out/test_INITIAL_01.caos ] \
    && (cd out && for f in test_INITIAL_01*; do diff -u "../expected/$f" "$f" || exit 1; done); then
    echo "PASS: output folder option"
else
    echo "FAIL: output folder option"
    failed=1
fi
rm -rf out

# #import under -p / -d combinations : the #include goes to the define header
# only when it exists (-d : none), whatever -p says (expected/import_<opt>/)
for opt in L Lp Ld Lpd; do
    rm -rf out && mkdir out
    if ../caos -$opt -o out test_IMPORT_01.caos \
        && [ "$(ls out | wc -l)" = "$(ls expected/import_$opt | wc -l)" ] \
        && (cd out && for f in *; do diff -u "../expected/import_$opt/$f" "$f" || exit 1; done); then
        echo "PASS: #import with -$opt"
    else
        echo "FAIL: #import with -$opt"
        failed=1
    fi
done
rm -rf out

# -M : the dependency file lists the generated files (-p, -d, -o and #struct
# are reflected) and the headers named by #import, without duplicates
# ("..." and $Name are listed, <...> is not); snapshots are expected/depend_<name>.d
depend_test() {
    name=$1
    input=$2
    shift 2
    rm -rf out && mkdir out
    if [ -n "$UPDATE" ]; then
        ../caos "$@" -M expected/depend_$name.d "$input" && rm -f test_DEPEND_0*.[ch]
        return 0
    fi
    if ../caos "$@" -M out/dep.d "$input" \
        && diff -u expected/depend_$name.d out/dep.d >&2; then
        echo "PASS: -M $name"
    else
        echo "FAIL: -M $name"
        failed=1
    fi
    rm -f test_DEPEND_0*.[ch]
}
depend_test L      test_DEPEND_01.caos -L
depend_test Lp     test_DEPEND_01.caos -Lp
depend_test Ld     test_DEPEND_01.caos -Ld
depend_test Lpd    test_DEPEND_01.caos -Lpd
depend_test Lo     test_DEPEND_01.caos -L -o out
depend_test struct test_DEPEND_02.caos -L
rm -rf out

# interfaces (iface/) : Reader is implemented by File and Mock, and main.c uses
# only Reader; output and dependency files match expected/iface/, then it is
# compiled and run
rm -rf out && mkdir out
for c in Reader File Mock; do
    ../caos -L -o out -M out/$c.cd iface/$c.caos || ifaceFail=1
done
if [ -n "$UPDATE" ]; then
    rm -rf expected/iface && mkdir expected/iface && cp out/* expected/iface/
elif [ -z "$ifaceFail" ] \
    && [ "$(ls out | wc -l)" = "$(ls expected/iface | wc -l)" ] \
    && (cd out && for f in *; do diff -u "../expected/iface/$f" "$f" || exit 1; done) \
    && gcc -Wall -Iout iface/main.c out/Reader.c out/File.c out/Mock.c -o out/app \
    && ./out/app; then
    echo "PASS: interface"
else
    echo "FAIL: interface"
    failed=1
fi
rm -rf out

# templates (generic/) : List<T> is written in List.caos and expanded for two
# types by #instance in Lists.caos; output and dependency file match
# expected/generic/, then it is compiled and run
rm -rf out && mkdir out
../caos -L -o out -M out/Lists.cd generic/Lists.caos || genericFail=1
if [ -n "$UPDATE" ]; then
    rm -rf expected/generic && mkdir expected/generic && cp out/* expected/generic/
elif [ -z "$genericFail" ] \
    && [ "$(ls out | wc -l)" = "$(ls expected/generic | wc -l)" ] \
    && (cd out && for f in *; do diff -u "../expected/generic/$f" "$f" || exit 1; done) \
    && gcc -Wall -Wextra -Iout generic/main.c out/Lists.c -o out/app \
    && ./out/app; then
    echo "PASS: templates"
else
    echo "FAIL: templates"
    failed=1
fi
rm -rf out

# option combinations on test_OPTION_01.caos (expected/option_<name>/) :
# the generated files match the snapshots, no other file is created,
# -L leaves out the #line directives, -C leaves out the // comments
# and -k renames the this pointer; the result is compiled and run
# unless -p / -d hide the header the struct lives in
option_test() {
    name=$1
    shift
    rm -rf out && mkdir out
    if [ -n "$UPDATE" ]; then
        ../caos "$@" -o out test_OPTION_01.caos && rm -rf expected/option_$name \
            && mkdir expected/option_$name && cp out/* expected/option_$name/
        return 0
    fi
    ok=0
    ../caos "$@" -o out test_OPTION_01.caos || ok=1
    [ "$(ls out | wc -l)" = "$(ls expected/option_$name | wc -l)" ] || ok=1
    for f in out/*; do
        diff -u "expected/option_$name/$(basename "$f")" "$f" >&2 || ok=1
    done
    case " $* " in
    *" -L"*) grep -q '^#line' out/test_OPTION_01.c && ok=1 ;;
    *) grep -q '^#line' out/test_OPTION_01.c || ok=1 ;;
    esac
    case " $* " in
    *" -C "*|*" -L -C "*) grep -q '//' out/test_OPTION_01.c && ok=1 ;;
    esac
    case " $* " in
    *" -k self "*) grep -q pthis out/test_OPTION_01.c && ok=1 ;;
    *) grep -q 'self' out/test_OPTION_01.c && ok=1 ;;
    esac
    case " $* " in
    *" -Lp "*|*" -Ld "*|*" -Lpd "*) ;;
    *) gcc out/test_OPTION_01.c -o out/run && ./out/run || ok=1 ;;
    esac
    if [ $ok = 0 ]; then
        echo "PASS: options $name"
    else
        echo "FAIL: options $name"
        failed=1
    fi
}
option_test L      -L
option_test lineno
option_test C      -L -C
option_test k      -L -k self
option_test Ck     -L -C -k self
option_test lineno_Ck -C -k self
option_test Lp     -Lp
option_test Ld     -Ld
option_test Lpd    -Lpd
rm -rf out

# self-hosting : caos regenerates its own classes (src/*.caos) with identical output
rm -rf out && mkdir out
selfhost() {
    opt=$1
    name=$2
    if ../caos "$opt" -o out "../src/$name.caos"; then
        for f in out/*; do
            diff -u "../src/$(basename "$f")" "$f" || return 1
        done
    else
        return 1
    fi
}
for c in Caos SBuf RBuf ListBuf Element SLib; do
    rm -rf out/*
    case $c in SLib) opt=-Lpd ;; *) opt=-Lp ;; esac
    if selfhost "$opt" "$c"; then
        echo "PASS: self-hosting ($c.caos)"
    else
        echo "FAIL: self-hosting ($c.caos)"
        failed=1
    fi
done
rm -rf out

exit $failed
