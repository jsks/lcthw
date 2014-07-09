#!/bin/zsh
. ./util/testf.sh

# Initialise our database
rm test.db 2>/dev/null
touch test.db

typeset -A OPTS
zparseopts -D -A OPTS r: d: || error "failed to parse cli args"
for i in ${(k)OPTS}; do
    case $i in
        ("-r")
            max_rows=$OPTS[$i];;
        ("-d")
            max_data=$OPTS[$i];;
    esac
done

: ${max_rows:=100}
: ${max_data:=512}

function test_set {
    for i in {0..$(($max_rows - 1))}; do
        ./ex17 test.db s $i "$(printf 'c%.0s' {1..$max_data})" "foo@bar" || return 1
    done
}

function test_get {
    for i in {0..$(($max_rows - 1))}; do
        ./ex17 test.db g $i || return 1
    done
}

stest "Creation" "./ex17 test.db c $max_rows $max_data"
stest "Set" test_set
stest "List" "./ex17 test.db l"
stest "Get" test_get
stest "Search" "./ex17 test.db f ccc"
stest "Delete" "./ex17 test.db d 1"
stest "Get[deleted id]" "./ex17 test.db g 1" 1
stest "Delete[ofb]" "./ex17 test.db d $(($max_rows+1))" 1
