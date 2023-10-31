#!/bin/sh

# This is a convenience script to build, clean and
# test this project.
VIXEN_BUILDSYS=`which meson`
VIXEN_BUILDDIR=build
VIXEN_DEVELOP_setupFLAGS=--buildtype=debug
VIXEN_RELEASE_setupFLAGS=--buildtype=release

set -eu

echo_help() {
    echo "\
usage: vixenw.sh [task(s)...]

Vixen Build Wrapper
A convenience script for managing this project.

Available Tasks:
build         Build this project executable, libs & tests.
build_release Build a release version of executable, libs & tests
clean         Remove files generated from build system.
test          Run all tests."
}

echo_performance() {
    echo "performing task $1"
}

build() {
    echo_performance build
    [[ ! -d $VIXEN_BUILDDIR ]] && $VIXEN_BUILDSYS setup $VIXEN_BUILDDIR $VIXEN_DEVELOP_setupFLAGS
    $VIXEN_BUILDSYS compile -C $VIXEN_BUILDDIR
}

build_release() {
    echo_performance build
    [[ ! -d ${VIXEN_BUILDDIR}_release ]] && $VIXEN_BUILDSYS setup ${VIXEN_BUILDDIR}_release $VIXEN_RELEASE_setupFLAGS
    $VIXEN_BUILDSYS compile -C $VIXEN_BUILDDIR
}

clean() {
    echo_performance clean
    [[ -d $VIXEN_BUILDDIR ]] && rm -rf $VIXEN_BUILDDIR
    [[ -d ${VIXEN_BUILDDIR}_release ]] && rm -rf ${VIXEN_BUILDDIR}_release
    find proto/ -type d -name "__pycache__" | xargs -Ixx rm -r xx
}

test() {
    echo_performance test
    build && time build/vixen_test $@
}

if [[ $# = 0 ]];
then
    echo_help
    exit 1
fi

for target in "$@"
do
    $target
    wait
done
