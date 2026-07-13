internal void fill_test_report_from_payload(Arena *arena, TestReport *test_report, TestPayload *payload)
{
    test_report->leak_count               = payload->leak_count;
    test_report->expected_value           = payload->expected_value;
    test_report->got_value                = payload->got_value;

    U64 buffer_offset;
    buffer_offset = 0;
    for(U64 string_index = 0; string_index < payload->expected_strings_sizes_count; string_index += 1)
    {
        U64 size = payload->expected_strings_sizes[string_index];
        U8 *destination = push_array(arena, U8, size);
        U8 *source      = payload->buffer_for_expected + buffer_offset;
        MemoryCopy(destination, source, size);
        buffer_offset += size;

        test_report->expected_strings[string_index] = (String8){ .str = destination, .size = size};
        test_report->expected_strings_count += 1;
    }

    buffer_offset = 0;
    for(U64 string_index = 0; string_index < payload->got_strings_sizes_count; string_index += 1)
    {
        U64 size = payload->got_strings_sizes[string_index];
        U8 *destination = push_array(arena, U8, size);
        U8 *source      = payload->buffer_for_got + buffer_offset;
        MemoryCopy(destination, source, size);
        buffer_offset += size;

        test_report->got_strings[string_index] = (String8){ .str = destination, .size = size};
        test_report->got_strings_count += 1;
    }
}

internal void run_and_evaluate_test_no_fork(Tester *tester, TestContext *test_context, U64 test_index)
{
    TestReport test_report = {0};
    TestPayload payload;

    // Set 'F' for failed test as the default.
    U8 character_to_print = 'F';

    TestParameters test_parameters = test_context->tests[test_index];
    payload = test_context->callback(test_parameters);

    if(payload.leak_count > 0)
    {
        test_report.flags     |= TestReportFlag_MemoryLeaked;
        tester->total_tests_leaked += 1;
        character_to_print = 'M';
    }

    if(!(payload.flags & TestPayloadFlag_ResultsMatch))
    {
        test_report.flags |= TestReportFlag_ResultsDoNotMatch;
    }

    if(!(payload.leak_count > 0) && (payload.flags & TestPayloadFlag_ResultsMatch))
    {
        test_report.flags |= TestReportFlag_TestPassed;
        tester->total_tests_passed += 1;
        character_to_print = '.';
    }

    if(!(test_report.flags & TestReportFlag_TestPassed)) // Only fill test report if test did not pass.
    {
        test_report.function_parameters_type = test_context->function_parameters_type;
        test_report.function_parameters      = test_context->tests[test_index];
        test_report.function_return_type     = test_context->function_return_type;

        fill_test_report_from_payload(tester->permanent_arena, &test_report, &payload);

        test_context->test_group.failed_test_reports[test_context->test_group.failed_test_reports_count] = test_report;
        test_context->test_group.failed_test_reports_count += 1;
        tester->total_tests_failed += 1;
    }

    if(tester->flags & TesterFlag_DisableColors)
    {
        Print("%c", character_to_print);
    }
    else
    {
        Print("%S", global_color_table_for_characters[character_to_print]);
    }
}

internal void run_and_evaluate_test(Tester *tester, TestContext *test_context, U64 test_index)
{
    TestReport test_report = {0};
    TestPayload payload    = {0};
    S64 bytes_read = 0;

    // Set 'F' for failed test as the default.
    U8 character_to_print = 'F';

    int pipefd[2];
    if(pipe(pipefd) != -1)
    {
        int pid = fork();
        if(pid == 0) // Child Process
        {
            close(pipefd[0]);

            dup2(tester->dev_null_fd, STDOUT_FILENO);
            dup2(tester->dev_null_fd, STDERR_FILENO);

            alarm(1);

            TestParameters test_parameters = test_context->tests[test_index];
            payload = test_context->callback(test_parameters);

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
                                test_report.flags     |= TestReportFlag_MemoryLeaked;
                                tester->total_tests_leaked += 1;
                                character_to_print = 'M';
                            }
                            if(!(payload.flags & TestPayloadFlag_ResultsMatch))
                            {
                                test_report.flags |= TestReportFlag_ResultsDoNotMatch;
                            }
                            if(!(payload.leak_count > 0) && (payload.flags & TestPayloadFlag_ResultsMatch))
                            {
                                test_report.flags |= TestReportFlag_TestPassed;
                                tester->total_tests_passed += 1;
                                character_to_print = '.';
                            }
                        }
                        else
                        {
                            test_report.flags |= TestReportFlag_ErrorPayloadRead;
                        }
                    }
                    else
                    {
                        test_report.flags     |= TestReportFlag_ErrorExitNonZero;
                        test_report.error_code = WEXITSTATUS(status);
                    }
                }
                else if(WIFSIGNALED(status))
                {
                    if(WTERMSIG(status) == SIGALRM)
                    {
                        test_report.flags |= TestReportFlag_ErrorTimeout;
                        tester->total_tests_timedout += 1;
                        character_to_print = 'T';
                    }
                    else
                    {
                        test_report.flags |= TestReportFlag_ErrorChildCrashed;
                        tester->total_tests_crashed += 1;
                        character_to_print = 'C';
                    }
                    test_report.error_code  = WTERMSIG(status);
                }
            }
            else
            {
                test_report.flags      |= TestReportFlag_ErrorErrnoSet;
                test_report.error_code  = errno;
            }
        }
        else
        {
            test_report.flags |= TestReportFlag_ErrorFork;
            close(pipefd[0]);
            close(pipefd[1]);
        }
    }
    else
    {
        test_report.flags |= TestReportFlag_ErrorPipe;
    }

    if(!(test_report.flags & TestReportFlag_TestPassed)) // Only fill test report if test did not pass.
    {
        test_report.function_parameters_type = test_context->function_parameters_type;
        test_report.function_parameters      = test_context->tests[test_index];
        test_report.function_return_type     = test_context->function_return_type;

        if(bytes_read == sizeof(payload))
        {
            fill_test_report_from_payload(tester->permanent_arena, &test_report, &payload);
        }

        test_context->test_group.failed_test_reports[test_context->test_group.failed_test_reports_count] = test_report;
        test_context->test_group.failed_test_reports_count += 1;
        tester->total_tests_failed += 1;
    }

    if(tester->flags & TesterFlag_DisableColors)
    {
        Print("%c", character_to_print);
    }
    else
    {
        Print("%S", global_color_table_for_characters[character_to_print]);
    }
}

internal String8 padding_for_stats(Arena *arena, U32 test_count)
{
    U64 some_extra_padding = 10;
    U64 padding_size   = TESTER_MAXIMUM_EXPECTED_TESTS_FOR_GROUP - test_count + some_extra_padding;
    U8 *padding_buffer = push_array(arena, U8, padding_size);
    MemorySet(padding_buffer, ' ', padding_size);
    String8 padding = (String8){.str = padding_buffer, .size = padding_size};
    return(padding);
}

internal void print_tests_statistics(U64 test_count, U64 failed_test_count, U32 tests_were_skipped)
{
    TemporaryArena scratch = ScratchArenaBegin(0);
    String8 stats;
    String8 padding;
    String8 stats_with_padding;

    if(tests_were_skipped)
    {
        stats   = push_string8_format(scratch.arena, String8Literal("%2u skipped"), test_count);
        padding = padding_for_stats(scratch.arena, global_symbol_missing_text.size - 5);
        stats_with_padding = push_string8_format(scratch.arena, String8Literal("%S%S%S\n"), global_symbol_missing_text, padding, stats);
    }
    else
    {
        stats   = push_string8_format(scratch.arena, String8Literal("%2u / %2u"), test_count - failed_test_count, test_count);
        padding = padding_for_stats(scratch.arena, test_count);
        stats_with_padding = push_string8_format(scratch.arena, String8Literal("%S%S\n"), padding, stats);
    }
    write(STDOUT_FILENO, stats_with_padding.str, stats_with_padding.size);

    ScratchArenaEnd(scratch);
}

internal void run_tests(Tester *tester, TestContext *test_context)
{
    Print("%-20S", test_context->test_group.name);

    U64 test_count_to_run = test_context->test_count;
    if(test_context->libft_function != 0)
    {
        tester->total_groups_tested += 1;

        if(tester->flags & TesterFlag_NoForkMode)
        {
            for(U32 test_index = 0; test_index < test_count_to_run; test_index += 1)
            {
                run_and_evaluate_test_no_fork(tester, test_context, test_index);
            }
        }
        else
        {
            for(U32 test_index = 0; test_index < test_count_to_run; test_index += 1)
            {
                run_and_evaluate_test(tester, test_context, test_index);
            }
        }
    }
    else
    {
        test_context->flags |= TestContextFlag_TestsWereSkipped;
        tester->total_tests_skipped += test_count_to_run;
    }

    print_tests_statistics(test_count_to_run, test_context->test_group.failed_test_reports_count, test_context->flags & TestContextFlag_TestsWereSkipped);

    if(test_context->test_group.failed_test_reports_count > 0)
    {
        tester->failed_groups[tester->failed_groups_count] = test_context->test_group;
        tester->failed_groups_count += 1;
    }
}
