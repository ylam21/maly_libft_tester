/*
 * Portions of this file are derived from RADDebugger (https://github.com/EpicGamesExt/raddebugger),
 * Copyright (c) Epic Games Tools.
 * Licensed under the MIT license.
 */
////////////////////////////////
// Arena Functions

// Arena creation/destruction
internal Arena *arena_create_(ArenaParams *params)
{
    U64 reserve_size = params->reserve_size;
    U64 commit_size  = params->commit_size;

    // round up reserve/commit sizes
    if(params->flags & ArenaFlag_LargePages)
    {
        reserve_size = AlignPow2(reserve_size, global_os_system_info.large_page_size);
        commit_size  = AlignPow2(commit_size,  global_os_system_info.large_page_size);
    }
    else
    {
        reserve_size = AlignPow2(reserve_size, global_os_system_info.page_size);
        commit_size  = AlignPow2(commit_size,  global_os_system_info.page_size);
    }

    // reserve/commit initial block
    void *base = 0;
    if(params->flags & ArenaFlag_LargePages)
    {
        base = os_reserve_large(reserve_size);
        os_commit_large(base, commit_size);
    }
    else
    {
        base = os_reserve_memory(reserve_size);
        os_commit_memory(base, commit_size);
    }

    // panic on arena creation failure
    if(base == 0)
    {
        os_write_message(String8Literal("[Arena Allocator Error] Unexpected memory allocation failure in arena_create_ .\n"));
        os_abort(1);
    }

    // extract arena header & fill
    Arena *arena        = base;
    arena->current      = arena;
    arena->previous     = 0;
    arena->commit_size  = params->commit_size;
    arena->reserve_size = params->reserve_size;
    arena->base_pos     = 0;
    arena->pos          = ARENA_HEADER_SIZE;
    arena->commited     = commit_size;
    arena->reserved     = reserve_size;
    arena->flags        = params->flags;

    return arena;
}

internal void arena_destroy(Arena *arena)
{
    Arena *current  = arena->current;
    Arena *previous = 0;

    while(current != 0)
    {
        previous = current->previous;
        os_release_memory(current, current->reserved);
        current  = previous;
    }
}

// Arena push/pop core functions
internal void *arena_push(Arena *arena, U64 size, U64 alignment, U32 zero)
{
    Arena *current = arena->current;
    U64 pos_pre    = AlignPow2(current->pos, alignment);
    U64 pos_post   = pos_pre + size;

    U64 size_to_zero = 0;
    if(zero)
    {
        size_to_zero = Min(pos_post, current->commited) - pos_pre;
    }

    // Create and chain new Arena, if there is not more reserved space on the current Arena
    if(current->reserved < pos_post && (arena->flags & ArenaFlag_NoChain) != 0)
    {
        U64 reserve_size = current->reserve_size;
        U64 commit_size  = current->commit_size;

        if(size + ARENA_HEADER_SIZE > reserve_size)
        {
            reserve_size = AlignPow2(size + ARENA_HEADER_SIZE, alignment);
            commit_size  = AlignPow2(size + ARENA_HEADER_SIZE, alignment);
        }

        Arena *new_arena = arena_create_(&(ArenaParams){ .reserve_size = reserve_size, .commit_size = commit_size, .flags = current->flags });
        size_to_zero = 0; // Newly allocated arenas already have zeroed commited pages, so it is unnecessarily to zero them again.

        new_arena->base_pos = current->base_pos + current->reserved;
        new_arena->previous = current;
        current             = new_arena;

        pos_pre  = AlignPow2(current->pos, alignment);
        pos_post = pos_pre + size;
    }

    // Commit new pages, if needed
    if(current->commited < pos_post)
    {
        U64 commit_post_aligned = pos_post + current->commit_size - 1;
        commit_post_aligned    -= commit_post_aligned % current->commit_size;
        U64 commit_post_smaller = Min(commit_post_aligned, current->reserved);
        U64 commit_size         = commit_post_smaller - current->commited;

        U8 *commit_ptr          = (U8 *)current + current->commited;
        if(current->flags & ArenaFlag_LargePages)
        {
            os_commit_large(commit_ptr, commit_size);
        }
        else
        {
            os_commit_memory(commit_ptr, commit_size);
        }
        current->commited = commit_post_smaller;
    }

    // Push onto current block
    void *result = 0;
    if(current->commited >= pos_post)
    {
        result       = (U8 *)current + pos_pre;
        current->pos = pos_post;
        MemoryZero(result, size_to_zero);
    }

    // Panic on failure
    if(result == 0)
    {
        os_write_message(String8Literal("[Arena Allocator Error] Unexpected memory allocation failure in arena_push.\n"));
        os_abort(1);
    }

    return result;
}

internal U64 arena_pos(Arena *arena)
{
    Arena *current = arena->current;
    U64 pos        = current->base_pos + current->pos;

    return(pos);
}

internal void arena_pop_to(Arena *arena, U64 pos)
{
    U64 bigger_pos = Max(ARENA_HEADER_SIZE, pos);

    Arena *current = arena->current;
    for(Arena *previous = 0; current->base_pos >= bigger_pos; current = previous)
    {
        previous = current->previous;
        os_release_memory(current, current->reserved);
    }
    arena->current = current;

    U64 new_pos = bigger_pos - current->base_pos;
    AssertAlways(new_pos <= current->pos);
    current->pos = new_pos;
}

// Arena pop helpers
internal void arena_clear(Arena *arena)
{
    arena_pop_to(arena, 0);
}

internal void arena_pop(Arena *arena, U64 amount)
{
    U64 pos_old = arena_pos(arena);
    U64 pos_new = pos_old;
    if(amount < pos_old)
    {
        pos_new = pos_old - amount;
    }
    arena_pop_to(arena, pos_new);
}

// Temporary arena scopes
internal TemporaryArena temporary_arena_begin(Arena *arena)
{
    U64 pos = arena_pos(arena);
    TemporaryArena temporary_arena = {.arena = arena, .saved_pos = pos};
    return temporary_arena;
}

internal void temporary_arena_end(TemporaryArena temporary_arena)
{
    arena_pop_to(temporary_arena.arena, temporary_arena.saved_pos);
}
