#ifndef BASE_PROFILER_H
#define BASE_PROFILER_H

#include <x86intrin.h>
#include <sys/time.h>

#define OS_TIMER_FREQUENCY Million(1)

#ifndef ENABLE_PROFILER
    #define ENABLE_PROFILER 0
#endif

typedef struct ProfilerBlock ProfilerBlock;
struct ProfilerBlock
{
	String8 name;
	U64     tsc_inclusive;
	U64     tsc_exclusive;
	U64     hit_count;
	U32     active_depth;
	U64     processed_byte_count;
};

typedef struct ProfilerReport ProfilerReport;
struct ProfilerReport
{
    String8     full_report;
    F64         total_time_elapsed_in_seconds;
};

typedef struct Profiler Profiler;
struct Profiler
{
	U64 os_time_start;
	U64 os_time_end;
	U64 profiler_tsc_start;
	U64 profiler_tsc_end;
};

#define PROFILER_MAX_BLOCK_COUNT KiB(4)
#define PROFILER_NULL_PARENT     0xffffffff

////////////////////////////////
// Globals
global ProfilerBlock profiler_blocks[PROFILER_MAX_BLOCK_COUNT] = {0};
global U32 profiler_current_parent = PROFILER_NULL_PARENT;
global Profiler profiler = {0};

internal_function void           profiler_start(void);
internal_function ProfilerReport profiler_end(Arena *arena);

#if ENABLE_PROFILER
    #define ProfilerBlockThroughputBegin(id_name, byte_count)                                  \
        local_persist const U32 prof_loc_##id_name = __COUNTER__;                              \
        profiler_blocks[prof_loc_##id_name].name = String8Literal(#id_name);                   \
        U32 prof_was_root_##id_name = (profiler_blocks[prof_loc_##id_name].active_depth == 0); \
        profiler_blocks[prof_loc_##id_name].active_depth += 1;                                 \
        profiler_blocks[prof_loc_##id_name].processed_byte_count += byte_count;                \
        U32 prof_parent_##id_name = profiler_current_parent;                                   \
        profiler_current_parent = prof_loc_##id_name;                                          \
        U64 prof_start_##id_name = __rdtsc()

    #define ProfilerBlockBegin(id_name) ProfilerBlockThroughputBegin(id_name, 0)

    #define ProfilerBlockEnd(id_name)                                                          \
        U64 prof_end_##id_name = __rdtsc();                                                    \
        U64 prof_elapsed_##id_name = prof_end_##id_name - prof_start_##id_name;                \
        profiler_blocks[prof_loc_##id_name].active_depth -= 1;                                 \
        if(prof_was_root_##id_name)                                                            \
        {                                                                                      \
            profiler_blocks[prof_loc_##id_name].tsc_inclusive += prof_elapsed_##id_name;       \
        }                                                                                      \
        profiler_blocks[prof_loc_##id_name].tsc_exclusive += prof_elapsed_##id_name;           \
        profiler_current_parent = prof_parent_##id_name;                                       \
        if(profiler_current_parent != PROFILER_NULL_PARENT)                                    \
        {                                                                                      \
	        profiler_blocks[profiler_current_parent].tsc_exclusive -= prof_elapsed_##id_name;  \
        }                                                                                      \
        profiler_blocks[prof_loc_##id_name].hit_count += 1
#else
    #define ProfilerBlockThroughputBegin(id_name, byte_count)
    #define ProfilerBlockBegin(id_name)
    #define ProfilerBlockEnd(id_name)
#endif // ENABLE_PROFILER

#endif // BASE_PROFILER_H
