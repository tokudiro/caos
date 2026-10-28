#!/bin/sh
# usage: test.sh  (run from any directory; requires ../caos to be built)
cd "$(dirname "$0")" || exit 1

rm -f test_*.c test_*.h

failed=0

# run_test <caos option> <test name>
run_test() {
    opt=$1
    name=test_$2
    if ../caos "$opt" "$name.caos" \
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
