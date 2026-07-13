/*
 * Portions of this file are derived from RADDebugger (https://github.com/EpicGamesExt/raddebugger),
 * Copyright (c) Epic Games Tools.
 * Licensed under the MIT license.
 */
#ifndef BASE_THREAD_CONTEXT_H
#define BASE_THREAD_CONTEXT_H

////////////////////////////////
// Base Per-Thread State
#define SCRATCH_ARENA_POOL_SIZE 2 // I have decided to go with size of 2. This number can be increased later anytime.

typedef struct ThreadContext ThreadContext;
struct ThreadContext
{
    Arena *scratch_arenas[SCRATCH_ARENA_POOL_SIZE]; // Array/Pool of scratch arenas
};

////////////////////////////////
// Thread Context Functions

// Thread-context initialization & release
internal void initialize_thread_context(void);
internal void thread_context_release(void);

// Scratch arenas
internal Arena *get_scratch_arena_from_thread_context(Arena *conflict_arena);

#define ScratchArenaBegin(conflict_arena) temporary_arena_begin(get_scratch_arena_from_thread_context(conflict_arena))
#define ScratchArenaEnd(scratch_arena)    temporary_arena_end(scratch_arena)

////////////////////////////////
// Globals
thread_static ThreadContext thread_local_thread_context;

#endif // BASE_THREAD_CONTEXT_H
