/*
 * Portions of this file are derived from RADDebugger (https://github.com/EpicGamesExt/raddebugger),
 * Copyright (c) Epic Games Tools.
 * Licensed under the MIT license.
 */
////////////////////////////////
// Thread Context Functions

// Thread-context initialization & release
internal_function
void initialize_thread_context(void)
{
    Arena **scratch_arena_pool  = thread_local_thread_context.scratch_arenas;
    for(U32 pool_index = 0; pool_index < SCRATCH_ARENA_POOL_SIZE; pool_index += 1)
    {
        scratch_arena_pool[pool_index] = arena_create();
    }
}

internal_function
void thread_context_release(void)
{
    Arena **scratch_arena_pool  = thread_local_thread_context.scratch_arenas;
    for(U32 pool_index = 0; pool_index < SCRATCH_ARENA_POOL_SIZE; pool_index += 1)
    {
        arena_destroy(scratch_arena_pool[pool_index]);
    }
}

// Scratch arenas
internal_function
Arena *get_scratch_arena_from_thread_context(Arena *conflict_arena)
{
    Arena **scratch_arena_pool  = thread_local_thread_context.scratch_arenas;
    Arena *result = 0;
    for(U32 pool_index = 0; pool_index < SCRATCH_ARENA_POOL_SIZE; pool_index += 1)
    {
        Arena *scratch_arena = scratch_arena_pool[pool_index];
        if(scratch_arena != conflict_arena)
        {
            result = scratch_arena;
            break;
        }
    }
    AssertAlways(result != 0);
    return result;
}
