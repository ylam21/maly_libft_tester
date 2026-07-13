#!/bin/bash

LIBFT_ARCHIVE=$1
if [ -z "$LIBFT_ARCHIVE" ]; then
    echo "Usage: ./build.sh <path/to/libft.a>"
    exit 1
fi

# Note: feel free to change this name
NAME="tester"

# Note: feel free to change this path
SOURCE_FILE="source/main.c"

# Note: Only gcc is supported. Do not change
COMPILER=gcc

# Note:
# -Wno-unused-function is used for some unused functions from base layer.
# -Wno-unused-variable is used for some unused globals from base layer.

# Note from Libft subject (Version 19.2):
# Some functions that you must reimplement, such as strlcpy, strlcat,
# and bzero, are not included by default in the GNU C Library (glibc).
# To test them against the system standard, you may need to include
# <bsd/string.h> and compile with the -lbsd flag.
# This behaviour is specific to glibc systems. If you are curious,
# take the opportunity to explore the differences between glibc and BSD libc.

"$COMPILER"  -Wall                                   \
             -Wextra                                 \
             -Werror                                 \
             -Wno-unused-function                    \
             -Wno-unused-variable                    \
             "$SOURCE_FILE"                          \
             -Wl,--whole-archive "$LIBFT_ARCHIVE"    \
             -Wl,--no-whole-archive                  \
             -Wl,--wrap=malloc                       \
             -Wl,--wrap=free                         \
             -Wl,--wrap=calloc                       \
             -Wl,--wrap=realloc                      \
             -lm                                     \
             -lbsd                                   \
             -o "$NAME"
