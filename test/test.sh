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

exit $failed
