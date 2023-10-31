#!/bin/sh

VIXEN_BUILDSYS=`which meson`
VIXEN_BUILDDIR=build/

set -eu

build() {
    [[ ! -d $VIXEN_BUILDDIR ]] && $VIXEN_BUILDSYS setup $VIXEN_BUILDDIR
    $VIXEN_BUILDSYS compile -C $VIXEN_BUILDDIR
}

clean() {
    [[ -d $VIXEN_BUILDDIR ]] && rm -rf $VIXEN_BUILDDIR
    find proto/ -type d -name "__pycache__" | xargs -Ixx rm -r xx
}

test() {
    build && time build/vixen_test $@
}

$@
