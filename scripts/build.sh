#!/usr/bin/env bash

#If you don't have access to make, but you do bash (odd), use this script to
#compile an executable from src/ that runs the program.

OPTIMIZATION_LEVEL='3'
STD_VERSION='c23'

main() {
    shopt -s globstar nullglob
    local -a source_files defs flags link

    source_files=(./src/**/*.c)
    defs=()
    flags=(-Isrc -std="$STD_VERSION" -O"$OPTIMIZATION_LEVEL" -Wall -Wextra)
    link=(-lncurses)

    mkdir -p bin
    clang "${source_files[@]}" "${defs[@]}" "${flags[@]}" "${link[@]}" -o bin/main
}

main "$@"
