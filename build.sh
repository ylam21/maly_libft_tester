#!/bin/bash

LIBFT_DIRECTORY=$1
if [ -z "$LIBFT_DIRECTORY" ]; then
    echo "Usage: ./build.sh <path/to/libft/directory>"
    exit 1
fi

COMPILER=gcc
NAME="tester"
TESTER_SOURCE_FILE="./source/main.c"

# OS Detection for BSD library
OS_NAME=$(uname -s)
BSD_FLAG=""
if [ "$OS_NAME" = "Linux" ]; then
    BSD_FLAG="-lbsd"
fi

LIBFT_SOURCE_FILES=$(find "$LIBFT_DIRECTORY" -maxdepth 1 -name "ft_*.c")

if [ -z "$LIBFT_SOURCE_FILES" ]; then
    echo "Error: No 'ft_*.c' files found in $LIBFT_DIRECTORY"
    exit 1
fi

$COMPILER  -Wall -Wextra                             \
           -Wno-unused-function                      \
           -Wno-unused-variable                      \
           -I"$LIBFT_DIRECTORY"                      \
           -Dmalloc=__wrap_malloc                    \
           -Dfree=__wrap_free                        \
           "$TESTER_SOURCE_FILE"                     \
           $LIBFT_SOURCE_FILES                       \
           -lm                                       \
           $BSD_FLAG                                 \
           -o "$NAME"

if [ $? -eq 0 ]; then
    echo "Build successful: $NAME"
else
    echo "Build failed."
    exit 1
fi
