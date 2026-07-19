#!/bin/bash

LIBFT_ARCHIVE_PATH=$1
if [ -z "$LIBFT_ARCHIVE_PATH" ]; then
    echo "Usage: ./build.sh <path/to/libft.a>"
    exit 1
fi

NAME="tester"
OS_NAME=$(uname -s)
COMPILER=gcc
CFLAGS="-Wall -Wextra -Werror -Wno-unused-function -Wno-unused-variable"
TESTER_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
TESTER_SOURCE_FILE="$TESTER_DIR/source/main.c"

if [[ "$OS_NAME" = "Linux" ]]; then
    LDFLAGS="-lm -lbsd"

    $COMPILER                                   \
    $CFLAGS                                     \
    $TESTER_SOURCE_FILE                         \
    -Wl,--whole-archive "$LIBFT_ARCHIVE_PATH"   \
    -Wl,--no-whole-archive                      \
    -Wl,--wrap=malloc                           \
    -Wl,--wrap=free                             \
    $LDFLAGS                                    \
    -o $NAME

elif [[ "$OS_NAME" = "Darwin" ]]; then
    LDFLAGS="-lm"

    $COMPILER                             \
    $CFLAGS                               \
    $TESTER_SOURCE_FILE                   \
    -Wl,-force_load,"$LIBFT_ARCHIVE_PATH" \
    $LDFLAGS                              \
    -o $NAME

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
