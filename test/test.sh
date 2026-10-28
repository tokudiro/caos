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
