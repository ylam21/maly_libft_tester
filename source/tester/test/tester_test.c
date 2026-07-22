// These are exclusively used by the isolated child processes
global sigjmp_buf global_jump_buffer;
global int        global_caught_signal;

internal_function void crash_handler(int signum)
{
    global_caught_signal = signum;
    siglongjmp(global_jump_buffer, 1);
}

read_only global TestGroup *global_test_groups[TESTER_MAXIMUM_TEST_GROUP_COUNT] =
{
    // Libc Functions
    [0]  = &test_group_ft_isalpha,
    [1]  = &test_group_ft_isdigit,
    [2]  = &test_group_ft_isalnum,
    [3]  = &test_group_ft_isascii,
    [4]  = &test_group_ft_isprint,
    [5]  = &test_group_ft_strlen,
    [6]  = &test_group_ft_memset,
    [7]  = &test_group_ft_bzero,
    [8]  = &test_group_ft_memcpy,
    [9]  = &test_group_ft_memmove,
    [10] = &test_group_ft_strlcpy,
    [11] = &test_group_ft_strlcat,
    [12] = &test_group_ft_toupper,
    [13] = &test_group_ft_tolower,
    [14] = &test_group_ft_strchr,
    [15] = &test_group_ft_strncmp,
    [16] = &test_group_ft_memchr,
    [17] = &test_group_ft_memcmp,
    [18] = &test_group_ft_strnstr,
    [19] = &test_group_ft_atoi,
    [20] = &test_group_ft_calloc,
    [21] = &test_group_ft_strdup,
    // Additional Functions
    [22] = &test_group_ft_substr,
    [23] = &test_group_ft_strjoin,
    [24] = &test_group_ft_strtrim,
    [25] = &test_group_ft_split,
    [26] = &test_group_ft_itoa,
    [27] = &test_group_ft_strmapi,
    [28] = &test_group_ft_striteri,
    [29] = &test_group_ft_putchar_fd,
    [30] = &test_group_ft_putstr_fd,
    [31] = &test_group_ft_putendl_fd,
    [32] = &test_group_ft_putnbr_fd,
    // Linked List Functions
    [33] = &test_group_ft_lstnew,
    [34] = &test_group_ft_lstadd_front,
    [35] = &test_group_ft_lstsize,
    [36] = &test_group_ft_lstlast,
    [37] = &test_group_ft_lstadd_back,
    [38] = &test_group_ft_lstdelone,
    [39] = &test_group_ft_lstclear,
    [40] = &test_group_ft_lstiter,
    [41] = &test_group_ft_lstmap,
};

internal_function
void run_all_tests_for_test_group_and_evaluate(TestWorkerContext *test_worker, TestGroup *test_group)
{
    TestPayload payloads[TESTER_MAXIMUM_TESTS_FOR_GROUP_COUNT] = {0};
    TestReportFlags flags = 0;
    S32 error_code = 0;

    int pipefd[2];
    if(pipe(pipefd) != -1)
    {
        int pid = fork();
        if(pid == 0) // Child Process
        {
            close(pipefd[0]);

            dup2(global_dev_null_fd, STDOUT_FILENO);
            dup2(global_dev_null_fd, STDERR_FILENO);

            struct sigaction sa = {0};
            sa.sa_handler = crash_handler;
            sigemptyset(&sa.sa_mask);
            sa.sa_flags = 0;
            sigaction(SIGSEGV, &sa, 0); // Segmentation Fault
            sigaction(SIGBUS,  &sa, 0); // Bus Error
            sigaction(SIGABRT, &sa, 0); // Double Free / Abort
            sigaction(SIGALRM, &sa, 0); // Timeout

            struct itimerval timeout = test_worker->timeout;
            struct itimerval cancel_timer = {0};

            for(U64 test_index = 0; test_index < test_group->test_count; test_index += 1)
            {
                global_caught_signal = 0;

                if(sigsetjmp(global_jump_buffer, 1) == 0)
                {
                    setitimer(ITIMER_REAL, &timeout, 0);

                    payloads[test_index] = test_group->callback(test_group->tests[test_index]);

                    setitimer(ITIMER_REAL, &cancel_timer, 0);
                }
                else
                {
                    payloads[test_index].flags = 0;
                    payloads[test_index].crash_signal = global_caught_signal;
                }
            }
            write(pipefd[1], payloads, sizeof(TestPayload) * test_group->test_count);
            close(pipefd[1]);
            exit(0);
        }
        else if(pid > 0) // Parent Process
        {
            close(pipefd[1]);

            S64 bytes_read = (S64)read(pipefd[0], payloads, sizeof(TestPayload) * test_group->test_count);
            close(pipefd[0]);
            if(bytes_read != (S64)(sizeof(TestPayload) * test_group->test_count))
            {
                flags |= TestReportFlag_ErrorPayloadRead;
            }

            int status;
            if(waitpid(pid, &status, 0) != -1)
            {
                if(WIFEXITED(status))
                {
                    if(WEXITSTATUS(status) != 0)
                    {
                        flags     |= TestReportFlag_ErrorExitNonZero;
                        error_code = WEXITSTATUS(status);
                    }
                }
                else if(WIFSIGNALED(status))
                {
                    if(WTERMSIG(status) == SIGALRM)
                    {
                        flags |= TestReportFlag_ErrorTimeout;
                    }
                    else
                    {
                        flags |= TestReportFlag_ErrorChildCrashed;
                    }
                    error_code  = WTERMSIG(status);
                }
            }
            else
            {
                flags      |= TestReportFlag_ErrorErrnoSet;
                error_code  = errno;
            }
        }
        else
        {
            flags |= TestReportFlag_ErrorFork;
            close(pipefd[0]);
            close(pipefd[1]);
        }
    }
    else
    {
        flags |= TestReportFlag_ErrorPipe;
    }

    if(!(flags & 0x007f))
    {
        TemporaryArena scratch = ScratchArenaBegin(0);

        U32 local_tests_passed_before = test_worker->local_tests_passed;

        U32 header_was_not_copied = 1;
        for(U64 payload_index = 0; payload_index < test_group->test_count; payload_index += 1)
        {
            U8 char_to_print = 'F';

            DebugInfoBuilder info =
            {
                .arena                 = scratch.arena,
                .worker                = test_worker,
                .group                 = test_group,
                .payload               = &payloads[payload_index],
                .char_to_print         = &char_to_print,
                .header_was_not_copied = &header_was_not_copied,
                .test_index            = payload_index,
            };
            String8 debug_info = debug_info_from_payload(&info);

            MemoryCopyString8(test_worker->local_test_groups_debug_report.str + test_worker->local_test_groups_debug_report.size, debug_info);
            test_worker->local_test_groups_debug_report.size += debug_info.size;

            // Copy the character_to_print
            if(test_worker->flags & TesterFlag_NoColors)
            {
                MemoryCopy(test_worker->local_test_groups_summary.str + test_worker->local_test_groups_summary.size, &char_to_print, sizeof(U8));
                test_worker->local_test_groups_summary.size += sizeof(U8);
            }
            else
            {
                MemoryCopyString8(test_worker->local_test_groups_summary.str + test_worker->local_test_groups_summary.size, global_color_table_for_characters[char_to_print]);
                test_worker->local_test_groups_summary.size += global_color_table_for_characters[char_to_print].size;
            }
        }

        U32 local_tests_passed_after = test_worker->local_tests_passed;
        U32 local_tests_failed  = test_group->test_count - (local_tests_passed_after - local_tests_passed_before);

        if(!header_was_not_copied)
        {
            String8 footer = push_string8_format(scratch.arena, String8Literal("(End of debug information for failed test group %S)\n"
                                                                               "(Total failed tests for this group was %u)\n\n"), test_group->name, local_tests_failed);
            // Copy the stats
            MemoryCopyString8(test_worker->local_test_groups_debug_report.str + test_worker->local_test_groups_debug_report.size, footer);
            test_worker->local_test_groups_debug_report.size += footer.size;
        }

        String8 stats   = push_string8_format(scratch.arena, String8Literal("%2u / %2u"), local_tests_passed_after - local_tests_passed_before, test_group->test_count);
        String8 padding = padding_for_stats(scratch.arena, test_group->test_count);
        String8 stats_with_padding = push_string8_format(scratch.arena, String8Literal("%S%S\n"), padding, stats);

        // Copy the stats
        MemoryCopyString8(test_worker->local_test_groups_summary.str + test_worker->local_test_groups_summary.size, stats_with_padding);
        test_worker->local_test_groups_summary.size += stats_with_padding.size;

        ScratchArenaEnd(scratch);
    }
    else
    {
        // TODO: handle this case better.
        (void)error_code;
        Print("Fatal Crash. Exiting Now.\n");
        exit(1);
    }
}

internal_function
String8 padding_for_stats(Arena *arena, U32 test_count)
{
    U64 some_extra_padding = 10;
    U64 padding_size   = TESTER_MAXIMUM_TESTS_FOR_GROUP_COUNT - test_count + some_extra_padding;
    U8 *padding_buffer = push_array(arena, U8, padding_size);
    MemorySet(padding_buffer, ' ', padding_size);
    String8 padding = (String8){.str = padding_buffer, .size = padding_size};
    return(padding);
}

internal_function
void *worker_thread_routine(void *params)
{
    initialize_thread_context(); // Since scratch arenas are thread-local specific, we need to call this function for every worker.
    TestWorkerContext *test_worker = (TestWorkerContext *)params;

    String8 tester_start_header = {0};
    String8 stats;
    String8 padding;
    String8 stats_with_padding;
    String8 name_with_padding;

    TemporaryArena scratch = ScratchArenaBegin(0);

    for(U64 test_group_index = test_worker->test_group_start_index; test_group_index < test_worker->test_group_end_index; test_group_index += 1)
    {
        if(test_group_index == 0)
        {
            tester_start_header = push_string8_format(scratch.arena, String8Literal("\n--- Testing for Libft Subject Version %S ---\n"
                                                                                    "\n--- Part 1 - Libc Functions ---\n"), tester_get_supported_libft_subject_version());
            MemoryCopyString8(test_worker->local_test_groups_summary.str + test_worker->local_test_groups_summary.size, tester_start_header);
            test_worker->local_test_groups_summary.size += tester_start_header.size;
        }
        else if(test_group_index == 22)
        {
            tester_start_header = push_string8_format(scratch.arena, String8Literal("\n--- Part 2 - Additional Functions ---\n"));
            MemoryCopyString8(test_worker->local_test_groups_summary.str + test_worker->local_test_groups_summary.size, tester_start_header);
            test_worker->local_test_groups_summary.size += tester_start_header.size;
        }
        else if(test_group_index == 33)
        {
            tester_start_header = push_string8_format(scratch.arena, String8Literal("\n--- Part 3 - Linked List Functions ---\n"));
            MemoryCopyString8(test_worker->local_test_groups_summary.str + test_worker->local_test_groups_summary.size, tester_start_header);
            test_worker->local_test_groups_summary.size += tester_start_header.size;
        }

        TestGroup *test_group = global_test_groups[test_group_index];

        // Copy the group name
        String8 name_with_padding = push_string8_format(scratch.arena, String8Literal("%-20S"), test_group->name);
        MemoryCopyString8(test_worker->local_test_groups_summary.str + test_worker->local_test_groups_summary.size, name_with_padding);
        test_worker->local_test_groups_summary.size += name_with_padding.size;

        if(test_group->libft_function != 0)
        {
            test_worker->local_test_groups_tested += 1;

            run_all_tests_for_test_group_and_evaluate(test_worker, test_group);
        }
        else
        {
            test_worker->local_tests_skipped += test_group->test_count;

            stats   = push_string8_format(scratch.arena, String8Literal("%2u skipped"), test_group->test_count);
            padding = padding_for_stats(scratch.arena, global_symbol_missing_text.size - 5);
            stats_with_padding = push_string8_format(scratch.arena, String8Literal("%S%S%S\n"), global_symbol_missing_text, padding, stats);

            // Copy the stats
            MemoryCopyString8(test_worker->local_test_groups_summary.str + test_worker->local_test_groups_summary.size, stats_with_padding);
            test_worker->local_test_groups_summary.size += stats_with_padding.size;
        }
    }

    ScratchArenaEnd(scratch);

    return(0);
}
