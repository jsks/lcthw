#!/bin/zsh
. ./util/testf.sh

trap "rm test.db 2>/dev/null" SIGEXIT

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

stest "Creation" ./ex17 test.db c $max_rows $max_data

for i in {0..$(($max_rows - 1))}; do
    stest "Set $i" ./ex17 test.db s $i "$(printf 'c%.0s' {1..$max_data})" foo@bar
done

stest "List" ./ex17 test.db l

for i in {0..$(($max_rows - 1))}; do
    stest "Get $i" ./ex17 test.db g $i
done

stest "Search" ./ex17 test.db f "ccc"
