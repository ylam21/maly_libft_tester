/*
 * Portions of this file are derived from RADDebugger (https://github.com/EpicGamesExt/raddebugger),
 * Copyright (c) Epic Games Tools.
 * Licensed under the MIT license.
 */
#ifndef BASE_CONTEXT_CRACKING_H
#define BASE_CONTEXT_CRACKING_H

// Compiler Cracking
#if defined(__clang__)
    #define COMPILER_CLANG 1
#elif defined(__GNUC__) || defined(__GNUG__)
    #define COMPILER_GCC   1
#else
    #error Compiler not supported. Only GCC or Clang are supported.
#endif

// Operating System Cracking
#if defined(__gnu_linux__) || defined(__linux__)
    #define OS_LINUX 1
    #define OS_POSIX 1
# elif defined(__APPLE__) && defined(__MACH__)
    #define OS_MAC 1
    #define OS_POSIX 1
#else
    #error Operating System not supported.
#endif

// Zero out undefined macros
#if !defined(COMPILER_GCC)
    #define COMPILER_GCC 0
#endif

#if !defined(OS_LINUX)
    #define OS_LINUX 0
#endif

#if !defined(OS_MAC)
    #define OS_MAC 0
#endif

#if !defined(OS_POSIX)
    #define OS_POSIX 0
#endif

#endif // BASE_CONTEXT_CRACKING_H
