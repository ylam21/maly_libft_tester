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
void run_and_evaluate_test(TestWorkerContext *test_worker, TestGroup *test_group, U64 test_index, U32 *header_was_not_copied)
{
    TestReportFlags flags = 0;
    S64 error_code = 0;
    TestPayload payload    = {0};
    S64 bytes_read         = 0;

    // Set 'F' for failed test as the default.
    U8 character_to_print = 'F';

    int pipefd[2];
    if(pipe(pipefd) != -1)
    {
        int pid = fork();
        if(pid == 0) // Child Process
        {
            close(pipefd[0]);

            dup2(global_dev_null_fd, STDOUT_FILENO);
            dup2(global_dev_null_fd, STDERR_FILENO);

            alarm(1);

            TestParameters test_parameters = test_group->tests[test_index];
            payload = test_group->callback(test_parameters);

            write(pipefd[1], &payload, sizeof(payload));
            close(pipefd[1]);
            exit(0);
        }
        else if(pid > 0) // Parent Process
        {
            close(pipefd[1]);
            bytes_read = (S64)read(pipefd[0], &payload, sizeof(payload));
            close(pipefd[0]);

            int status;
            if(waitpid(pid, &status, 0) != -1)
            {
                if(WIFEXITED(status))
                {
                    if(WEXITSTATUS(status) == 0)
                    {
                        if(bytes_read == sizeof(payload))
                        {
                            if(payload.leak_count > 0)
                            {
                                flags |= TestReportFlag_MemoryLeaked;
                                test_worker->local_tests_leaked += 1;
                                character_to_print = 'M';
                            }
                            if(!(payload.flags & TestPayloadFlag_ResultsMatch))
                            {
                                flags |= TestReportFlag_ResultsDoNotMatch;
                            }
                            if(!(payload.leak_count > 0) && (payload.flags & TestPayloadFlag_ResultsMatch))
                            {
                                flags |= TestReportFlag_TestPassed;
                                test_worker->local_tests_passed += 1;
                                character_to_print = '.';
                            }
                        }
                        else
                        {
                            flags |= TestReportFlag_ErrorPayloadRead;
                        }
                    }
                    else
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
                        test_worker->local_tests_timedout += 1;
                        character_to_print = 'T';
                    }
                    else
                    {
                        flags |= TestReportFlag_ErrorChildCrashed;
                        test_worker->local_tests_crashed += 1;
                        character_to_print = 'C';
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


    if(!(flags & TestReportFlag_TestPassed))
    {
        test_worker->local_tests_failed += 1;

        TemporaryArena scratch = ScratchArenaBegin(0);

        TestReport test_report =
        {
            .flags        = flags,
            .error_code   = error_code,
            .test_group   = test_group,
            .test_payload = &payload,
            .test_index   = test_index,
        };
        String8 debug_info = debug_info_from_test_report(scratch.arena, &test_report, header_was_not_copied);
        MemoryCopyString8(test_worker->local_test_groups_report.str + test_worker->local_test_groups_report.size, debug_info);
        test_worker->local_test_groups_report.size += debug_info.size;

        ScratchArenaEnd(scratch);
    }

    // Copy the character_to_print
    if(test_worker->flags & TesterFlag_NoColors)
    {
        MemoryCopy(test_worker->local_test_groups_summary.str + test_worker->local_test_groups_summary.size, &character_to_print, sizeof(U8));
        test_worker->local_test_groups_summary.size += sizeof(U8);
    }
    else
    {
        MemoryCopyString8(test_worker->local_test_groups_summary.str + test_worker->local_test_groups_summary.size, global_color_table_for_characters[character_to_print]);
        test_worker->local_test_groups_summary.size += global_color_table_for_characters[character_to_print].size;
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
    TestWorkerContext *test_worker = (TestWorkerContext *)params;

    String8 tester_start_header;
    String8 stats;
    String8 padding;
    String8 stats_with_padding;
    String8 name_with_padding;

    TemporaryArena scratch = ScratchArenaBegin(0);
    if(test_worker->test_group_start_index == 0)
    {
        tester_start_header = push_string8_format(scratch.arena, String8Literal("\n--- Testing for Libft Subject Version %S ---\n"
                                                                                "\n--- Part 1 - Libc Functions ---\n"), tester_get_supported_libft_subject_version());
    }
    else if(test_worker->test_group_start_index == 22)
    {
        tester_start_header = push_string8_format(scratch.arena, String8Literal("\n--- Part 2 - Additional Functions ---\n"));
    }
    else if(test_worker->test_group_start_index == 33)
    {
        tester_start_header = push_string8_format(scratch.arena, String8Literal("\n--- Part 3 - Linked List Functions ---\n"));
    }

    MemoryCopyString8(test_worker->local_test_groups_summary.str + test_worker->local_test_groups_summary.size, tester_start_header);
    test_worker->local_test_groups_summary.size += tester_start_header.size;

    for(U64 test_group_index = test_worker->test_group_start_index; test_group_index < test_worker->test_group_end_index; test_group_index += 1)
    {
        TestGroup *test_group = global_test_groups[test_group_index];

        U32 test_count = test_group->test_count;
        U32 local_tests_passed_before = test_worker->local_tests_passed;

        // Copy the group name
        name_with_padding = push_string8_format(scratch.arena, String8Literal("%-20S"), test_group->name);
        MemoryCopyString8(test_worker->local_test_groups_summary.str + test_worker->local_test_groups_summary.size, name_with_padding);
        test_worker->local_test_groups_summary.size += name_with_padding.size;

        if(test_group->libft_function != 0)
        {
            U32 header_was_not_copied = 1;
            test_worker->local_test_groups_tested += 1;
            for(U64 test_index = 0; test_index < test_count; test_index += 1)
            {
                run_and_evaluate_test(test_worker, test_group, test_index, &header_was_not_copied);
            }
            U32 local_tests_passed_after = test_worker->local_tests_passed;

            U32 local_failed_test_count = test_count - (local_tests_passed_after - local_tests_passed_before);
            if(local_failed_test_count != 0) // Copy footer
            {
                String8 footer = push_string8_format(scratch.arena, String8Literal("(End of debug information for failed test group %S\n)"
                                                                                   "Total failed tests for this group was %u\n)"
                                                                                   "----------------------------------------\n"), test_group->name, local_failed_test_count);
                // Copy the stats
                MemoryCopyString8(test_worker->local_test_groups_report.str + test_worker->local_test_groups_report.size, footer);
                test_worker->local_test_groups_report.size += footer.size;
            }

            stats   = push_string8_format(scratch.arena, String8Literal("%2u / %2u"), local_tests_passed_after - local_tests_passed_before, test_count);
            padding = padding_for_stats(scratch.arena, test_count);
            stats_with_padding = push_string8_format(scratch.arena, String8Literal("%S%S\n"), padding, stats);
        }
        else
        {
            test_worker->local_tests_skipped += test_count;
            stats   = push_string8_format(scratch.arena, String8Literal("%2u skipped"), test_count);
            padding = padding_for_stats(scratch.arena, global_symbol_missing_text.size - 5);
            stats_with_padding = push_string8_format(scratch.arena, String8Literal("%S%S%S\n"), global_symbol_missing_text, padding, stats);
        }

        // Copy the stats
        MemoryCopyString8(test_worker->local_test_groups_summary.str + test_worker->local_test_groups_summary.size, stats_with_padding);
        test_worker->local_test_groups_summary.size += stats_with_padding.size;
    }

    ScratchArenaEnd(scratch);

    return(0);
}
