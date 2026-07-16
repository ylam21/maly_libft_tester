#ifndef TESTER_DEBUG_INFORMATION_BUILDER_H
#define TESTER_DEBUG_INFORMATION_BUILDER_H

#define TESTER_MAXIMUM_VIEW_BYTES_FOR_PRINT 64

typedef struct DebugInfoBuilder DebugInfoBuilder;
struct DebugInfoBuilder
{
    Arena             *arena;
    TestWorkerContext *worker;
    TestGroup         *group;
    TestPayload       *payload;
    U8                *char_to_print;
    U32               *header_was_not_copied;
    U32               test_index;
};

// Tester Debug Information Builders Main
internal_function U64     push_function_parameters_from_test_group(Arena *arena, TestGroup *test_group, U32 test_index);
internal_function U64     push_results_from_test_payload(Arena *arena, TestGroup *test_group, TestPayload *payload);
internal_function String8 debug_info_from_payload(DebugInfoBuilder *info);

// Tester Debug Information Builders Helpers
internal_function String8 get_memory_view(Arena *arena, void *memory_block);
internal_function String8 get_string_view(Arena *arena, void *memory_block);


#endif // TESTER_DEBUG_INFORMATION_BUILDER_H
