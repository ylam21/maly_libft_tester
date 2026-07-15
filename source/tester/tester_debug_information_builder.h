#ifndef TESTER_DEBUG_INFORMATION_BUILDER_H
#define TESTER_DEBUG_INFORMATION_BUILDER_H

#define TESTER_MAXIMUM_VIEW_BYTES_FOR_PRINT 64

// Tester Debug Information Builders
internal_function U32     build_debug_information_from_tester_and_write_to_fd(int fd, Tester *tester);
internal_function U64     push_debug_information_from_test_report(Arena *arena, TestReport *test_report, TestGroup *test_group);
internal_function U64     push_general_error_messages_from_test_report(Arena *arena, TestReport *test_report, TestGroup *test_group);
internal_function U64     push_results_from_test_report(Arena *arena, TestReport *test_report);
internal_function U64     push_function_parameters_from_test_report(Arena *arena, TestReport *test_report);
internal_function String8 get_memory_view(Arena *arena, void *memory_block);
internal_function String8 get_string_view(Arena *arena, void *memory_block);

#endif // TESTER_DEBUG_INFORMATION_BUILDER_H
