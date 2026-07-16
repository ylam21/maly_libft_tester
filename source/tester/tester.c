// Tester Functions
internal_function
String8 tester_get_version(void)
{
    return(global_tester_version);
}

internal_function
String8 tester_get_supported_libft_subject_version(void)
{
    return(global_tester_supported_libft_subject_version);
}

internal_function
Tester initialize_tester(void)
{
    Tester tester = {0};

    tester.permanent_arena = arena_create();
    tester.flags           = TESTER_DEFAULT_FLAGS;
    tester.output_filename = TESTER_DEFAULT_OUTPUT_FILENAME;

    global_dev_null_fd = open("/dev/null", O_WRONLY);

    return(tester);
}

internal_function
void tester_run(Tester *tester)
{
    U64 summary_size_for_test_group      = (tester->flags & TesterFlag_NoColors) ? 250 : 500; // Need to calculate this
    U64 summary_size_for_all_test_groups = TESTER_MAXIMUM_TEST_GROUP_COUNT * summary_size_for_test_group;
    U8 *shared_test_group_summary_start  = push_array(tester->permanent_arena, U8, summary_size_for_all_test_groups);

    U64 report_size_for_test            = 1024; // these should be defined macros of global read_only variables
    U64 report_size_for_test_group      = TESTER_MAXIMUM_TESTS_FOR_GROUP_COUNT  * report_size_for_test;
    U64 report_size_for_all_test_groups = TESTER_MAXIMUM_TEST_GROUP_COUNT * report_size_for_test_group;
    U8 * shared_test_group_report_start = push_array(tester->permanent_arena, U8, report_size_for_all_test_groups);

    S64 core_count   = sysconf(_SC_NPROCESSORS_ONLN);
    U64 thread_count = MaximumBetween(4, core_count);

    pthread_t *threads         = push_array(tester->permanent_arena, pthread_t        , thread_count);
    TestWorkerContext *workers = push_array(tester->permanent_arena, TestWorkerContext, thread_count);

    U32 test_group_count = TESTER_MAXIMUM_TEST_GROUP_COUNT;
    U64 chunk_size       = test_group_count / thread_count;
    U64 remainder        = test_group_count % thread_count;

    U64 current_test_group_index = 0;
    for(U64 thread_index = 0; thread_index < thread_count; thread_index += 1)
    {
        workers[thread_index].test_group_start_index = current_test_group_index;
        U64 test_groups_for_this_thread = chunk_size + (thread_index < remainder ? 1 : 0); // why 1:0 and not 3:0 or any other x:0?
        workers[thread_index].test_group_end_index = current_test_group_index + test_groups_for_this_thread;

        workers[thread_index].local_test_groups_summary.str = shared_test_group_summary_start + (summary_size_for_test_group * current_test_group_index);
        workers[thread_index].local_test_groups_report.str  = shared_test_group_report_start  + (report_size_for_test_group  * current_test_group_index);

        current_test_group_index += test_groups_for_this_thread;

        pthread_create(&threads[thread_index], 0, worker_thread_routine, &workers[thread_index]);
    }

    U8  *test_groups_report_buffer = push_array(tester->permanent_arena, U8, report_size_for_all_test_groups);
    U64  test_groups_report_buffer_offset = 0;

    for(U64 thread_index = 0; thread_index < thread_count; thread_index += 1)
    {
        pthread_join(threads[thread_index], 0);

        tester->total_test_groups_tested += workers[thread_index].local_test_groups_tested;
        tester->total_tests_passed       += workers[thread_index].local_tests_passed;
        tester->total_tests_failed       += workers[thread_index].local_tests_failed;
        tester->total_tests_leaked       += workers[thread_index].local_tests_leaked;
        tester->total_tests_crashed      += workers[thread_index].local_tests_crashed;
        tester->total_tests_timedout     += workers[thread_index].local_tests_timedout;
        tester->total_tests_skipped      += workers[thread_index].local_tests_skipped;

        MemoryCopyString8(test_groups_report_buffer + test_groups_report_buffer_offset, workers[thread_index].local_test_groups_report);
        test_groups_report_buffer_offset += workers[thread_index].local_test_groups_report.size;
        Assert(workers[thread_index].local_test_groups_report.size < (report_size_for_test_group * workers[thread_index].local_test_groups_tested));

        write(STDOUT_FILENO, workers[thread_index].local_test_groups_summary.str, workers[thread_index].local_test_groups_summary.size);
    }

    tester->report.str  = test_groups_report_buffer;
    tester->report.size = test_groups_report_buffer_offset;
}
