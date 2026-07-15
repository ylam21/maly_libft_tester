#!/bin/bash

LIBFT_ARCHIVE=$1
if [ -z "$LIBFT_ARCHIVE" ]; then
    echo "Usage: ./build.sh <path/to/libft.a>"
    exit 1
fi

# Note: Only GCC is supported. Do not change.
COMPILER=gcc

# Note: Feel free to change this name.
NAME="tester"

# Note: Feel free to change this path.
#       I recommend to change this to a full path.
SOURCE_FILE="/home/omaly/repos/maly_libft_tester/source/main.c"


# -----------------------------------------------------------------------------
# COMPILER & LINKER FLAGS EXPLANATION
# -----------------------------------------------------------------------------
# STANDARD COMPILATION
# -Wall -Wextra -Werror:
#     Standard strict compilation flags.
#
# -Wno-unused-function / -Wno-unused-variable:
#     Suppresses specific warnings for unused functions and globals.
#     (defined tester's base layer).
#
# ARCHIVE INCLUSION OVERRIDES
# -Wl,--whole-archive "$LIBFT_ARCHIVE"
# -Wl,--no-whole-archive
#     By default, the linker only extracts object files from an archive (.a)
#     if they resolve a currently undefined reference. Because the tester uses
#     '#pragma weak' symbols, the linker wouldn't see them as strictly "required"
#     and would drop your libft functions, causing false "symbol missing" errors.
#     `--whole-archive` forces the linker to blindly pack EVERY object file from
#     your libft.a into the tester. `--no-whole-archive` immediately toggles this
#     behavior off so subsequent system libraries aren't bloated into your executable.
#
# MEMORY TRACKING WRAPPERS
# -Wl,--wrap=malloc (along with free, calloc, realloc):
#     Instructs the linker to intercept all calls to standard memory functions.
#     Any call to `malloc` inside libft is redirected to your custom `__wrap_malloc`
#     function (defined in tester_memory_tracking.c). This allows the tester to
#     count allocations and track memory leaks across tests.
#
# EXTERNAL LIBRARIES
# -lm:
#     Links the system math library (libm). Required to resolve floating-point math operations.
#     (provided by the tester's base layer)
#
# -lbsd:
#     Links the BSD standard library. Required on Linux/glibc systems to provide
#     the system standard implementations of `strlcpy` and `strlcat` for the
#     tester to evaluate your libft against.
# -----------------------------------------------------------------------------

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

if [ $? -eq 0 ]; then
    echo "Build successful: $NAME"
else
    echo "Build failed."
    exit 1
fi
