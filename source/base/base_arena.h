/*
 * Portions of this file are derived from RADDebugger (https://github.com/EpicGamesExt/raddebugger),
 * Copyright (c) Epic Games Tools.
 * Licensed under the MIT license.
 */
#ifndef BASE_ARENA_H
#define BASE_ARENA_H

////////////////////////////////
// Arena Types

typedef U64 ArenaFlags;
enum
{
    ArenaFlag_NoChain    = (1<<0),
    ArenaFlag_LargePages = (1<<1),
};

typedef struct ArenaParams ArenaParams;
struct ArenaParams
{
    ArenaFlags  flags;
    U64         reserve_size;
    U64         commit_size;
};

#define ARENA_HEADER_SIZE 128
typedef struct Arena Arena;
struct Arena
{
    Arena *current;
    Arena *previous;
    U64 commit_size;
    U64 reserve_size;
    U64 base_pos;
    U64 pos;
    U64 commited;
    U64 reserved;
    ArenaFlags flags;
};
StaticAssert(sizeof(Arena) <= ARENA_HEADER_SIZE, arena_header_size_check);

typedef struct TemporaryArena TemporaryArena;
struct TemporaryArena
{
    Arena *arena;
    U64 saved_pos;
};

////////////////////////////////
// Arena Functions
#define ARENA_DEFAULT_RESERVE_SIZE GiB(64)
#define ARENA_DEFAULT_COMMIT_SIZE  KiB(64)
#define ARENA_DEFAULT_FLAGS        0

// Arena creation/destruction
internal_function Arena *arena_create_(ArenaParams *params);
#define arena_create(...) arena_create_(&(ArenaParams){ .reserve_size = ARENA_DEFAULT_RESERVE_SIZE, .commit_size = ARENA_DEFAULT_COMMIT_SIZE, .flags = ARENA_DEFAULT_FLAGS})
internal_function void arena_destroy(Arena *arena);

// Arena push/pop/pos core functions
internal_function void *arena_push(Arena *arena, U64 size, U64 alignment, U32 zero);
internal_function U64   arena_pos(Arena *arena);
internal_function void  arena_pop_to(Arena *arena, U64 pos);

// Arena pop helpers
internal_function void arena_clear(Arena *arena);
internal_function void arena_pop(Arena *arena, U64 amount);

// Temporary arena scopes
internal_function TemporaryArena temporary_arena_begin(Arena *arena);
internal_function void temporary_arena_end(TemporaryArena temporary_arena);

// Push helper macros
#define push_array_no_zero_aligned(arena, data_type, count, alignment) (data_type *)arena_push((arena), sizeof(data_type)*(count), (alignment), 0)
#define push_array_aligned(arena, data_type, count, alignment)         (data_type *)arena_push((arena), sizeof(data_type)*(count), (alignment), 1)
#define push_array_no_zero(arena, data_type, count)                    push_array_no_zero_aligned(arena, data_type, count, Max(8, AlignmentOf(data_type)))
#define push_array(arena, data_type, count)                            push_array_aligned(arena, data_type, count, Max(8, AlignmentOf(data_type)))

#endif // BASE_ARENA_H
