#!/bin/zsh

function fail {
    print "Fail"
    exit 113
}

# Initialise our database
rm test.db 2>/dev/null
touch test.db

integer verbose 
exec {verbose}>/dev/null

typeset -A OPTS
zparseopts -D -A OPTS r: d: v || fail
for i in ${(k)OPTS}; do
    case $i in
        ("-r")
            max_rows=$OPTS[$i];;
        ("-d")
            max_data=$OPTS[$i];;
        ("-v")
            exec {verbose}>&1;;
    esac
done

: ${max_rows:=100}
: ${max_data:=512}

print -n "Creation: "
./ex17 test.db c $max_rows $max_data >&$verbose || fail
print "Pass"

print -n "Set: "
for i in {0..$(($max_rows - 1))}; do
    ./ex17 test.db s $i "$(printf 'c%.0s' {1..$max_data})" foo@bar >&$verbose || fail
done
print "Pass"

print -n "List: "
./ex17 test.db l >&$verbose || fail
print "Pass"

print -n "Get: "
for i in {0..$(($max_rows - 1))}; do
    ./ex17 test.db g $i >&$verbose || fail
done
print "Pass"

print -n "Search: "
./ex17 test.db f "ccc" >&$verbose || fail
print "Pass"
