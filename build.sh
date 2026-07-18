#!/bin/bash

INPUT_PATH=$1
NAME="tester"
OS_NAME=$(uname -s)
COMPILER=gcc
CFLAGS="-Wall -Wextra -Werror -Wno-unused-function -Wno-unused-variable"
TESTER_SOURCE_FILE="./source/main.c" # Note: I recommend to change this to an absolute path.

if [[ "$OS_NAME" = "Linux" ]]; then
    if [[ -f "$INPUT_PATH" ]]; then
        LDFLAGS="-lm -lbsd"

        $COMPILER                         \
        $CFLAGS                           \
        $TESTER_SOURCE_FILE               \
        -Wl,--whole-archive "$INPUT_PATH" \
        -Wl,--no-whole-archive            \
        -Wl,--wrap=malloc                 \
        -Wl,--wrap=free                   \
        $LDFLAGS                          \
        -o $NAME
    else
        echo "Usage: ./build.sh <path/to/libft.a>"
        exit 1
    fi

elif [[ "$OS_NAME" = "Darwin" ]]; then
    if [[ -d "$INPUT_PATH" ]]; then
        LDFLAGS="-lm"
        LIBFT_SOURCE_FILES=$(find "$INPUT_PATH" -maxdepth 1 -name "ft_*.c")

        if [[ -z "$LIBFT_SOURCE_FILES" ]]; then
            echo "Error: No 'ft_*.c' files found in $INPUT_PATH"
            exit 1
        fi

        $COMPILER              \
        $CFLAGS                \
        -fPIE                  \
        -I"$INPUT_PATH"        \
        -Dmalloc=__wrap_malloc \
        -Dfree=__wrap_free     \
        $TESTER_SOURCE_FILE    \
        $LIBFT_SOURCE_FILES    \
        $LDFLAGS               \
        -o $NAME
    else
        echo "Usage: ./build.sh <path/to/libft/directory>"
        exit 1
    fi
else
    echo "Error: OS '$OS_NAME' not supported."
    exit 1
fi

if [[ $? -eq 0 ]]; then
    echo "Build successful: $NAME"
else
    echo "Build failed."
    exit 1
fi
