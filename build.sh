#!/bin/bash

BUILD_DIR=build

RUN_MAIN=false
RUN_TESTS=false
CLEAN_BUILD=false

function print_title {
    printf "\n=== $1 ===\n\n"
}

function exit_on_failure() {
    if [ $? -ne 0 ]; then
        printf "${RED}X\n"
        echo "Error: $1 failed. Exiting."
        exit 1
    fi
}

# Parse flags
while getopts "rtc" flag; do
    case $flag in
        c) CLEAN_BUILD=true ;;
        r) RUN_MAIN=true ;;
        t) RUN_TESTS=true ;;
        *)
            echo "Usage: $0 [-r] [-t] [-c]"
            echo "  -r: Run main"
            echo "  -t: Run tests"
            echo "  -c: Clean build directory"
            exit 1
            ;;
    esac
done

if [ "$CLEAN_BUILD" = true ]; then
    echo "Cleaning build directory"
    rm -rf $BUILD_DIR
    exit_on_failure "Clean build directory"
fi

mkdir -p $BUILD_DIR

cd $BUILD_DIR

cmake -S ..
exit_on_failure "Cmake"

make
exit_on_failure "Make"

if [ "$RUN_TESTS" = true ]; then
    print_title "Running tests"
    ctest
    exit_on_failure "Tests"
fi

if [ "$RUN_MAIN" = true ]; then
    print_title "Running main"
    ./Text
    exit_on_failure "Main"
fi
