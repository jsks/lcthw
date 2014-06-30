#!/usr/bin/zsh
# Ridiculously simple testing framework

autoload colors
(( $terminfo[colors] >= 8 )) && colors

function error {
    print "$fg[red]Error -> $*$fg[white]" >&2
    exit 115
}

function fail {
    print "$fg[red]Test failed$fg[white]" >&2
    exit 113
}

function stest {
    print -n "$fg[white]$1: "
    $@[2,-1] 2>&$verbose_err 1>&$debug || fail
    print "$fg[green]Passed$fg[white]"
}

function timer {
    print "$fg[white]Timing: $1"
    time ( $@[2,-1] >&$debug 2>&$verbose_err )
}

integer verbose_err debug
exec {verbose_err}>/dev/null
exec {debug}>/dev/null

local -a __OPTS
zparseopts -D -a __OPTS -E -- vv v || error "failed to parse cli args"
case $__OPTS in
    ("-v")
        exec {verbose_err}>&2;;
    ("-vv")
        exec {debug}>&1;;
esac

unset __OPTS
