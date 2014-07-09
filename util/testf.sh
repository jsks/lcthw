#!/usr/bin/zsh
# Ridiculously simple testing framework
# stest:
#   $1 =  Header string
#   $2 =  Cmd to run
#   $3 =  Expected return value to test
setopt local_options

autoload colors
(( $terminfo[colors] >= 8 )) && colors

function error {
    print "$fg[red]Error -> $*$fg[white]" >&2
    exit 115
}

function fail {
    print "$fg[red]Failed$fg[white]" >&2
    exit 113
}

function pass {
    print "$fg[green]Passed$fg[white]"
}

function stest {
    print -n "$fg[white]$1: "
    print -n "$fg[blue]($2)$fg[white] " >&$debug

    ${(z)2} 2>&$verbose_err 1>&$debug

    [[ $? != ${3:-0} ]] && fail || pass
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
        exec {verbose_err}>&2
        exec {debug}>&1;;
esac

unset __OPTS
