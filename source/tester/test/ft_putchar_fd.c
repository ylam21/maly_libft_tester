internal_function
TestPayload callback_for_putchar_fd(TestParameters test_parameters)
{
    TestPayload payload = {0};
    TestParameters_CharPtr parameters = test_parameters.char_ptr;

    char *expected_string = (char *)parameters.ptr;
    U64 expected_length = length_of_cstring(expected_string);

    MemoryCopy(payload.buffer_for_expected, expected_string, expected_length + 1);
    payload.expected_strings_sizes[0]    = expected_length + 1;
    payload.expected_strings_sizes_count = 1;

    global_allocation_count = 0;

    int pipe_fd[2];
    if(pipe(pipe_fd) != -1)
    {
        // Call the libft function.
        ft_putchar_fd((char)parameters.c, pipe_fd[1]);

        close(pipe_fd[1]);

        char got_buffer[TESTER_MAXIMUM_PAYLOAD_BUFFER_SIZE];
        MemoryZero(got_buffer, TESTER_MAXIMUM_PAYLOAD_BUFFER_SIZE);

        int bytes_read = read(pipe_fd[0], got_buffer, TESTER_MAXIMUM_PAYLOAD_BUFFER_SIZE - 1);
        close(pipe_fd[0]);

        if(bytes_read >= 0)
        {
            got_buffer[bytes_read] = '\0';

            U64 got_length = bytes_read;
            MemoryCopy(payload.buffer_for_got, got_buffer, got_length + 1);
            payload.got_strings_sizes[0]    = got_length + 1;
            payload.got_strings_sizes_count = 1;

            if((got_length == expected_length) && MemoryMatch(payload.buffer_for_got, payload.buffer_for_expected, expected_length))
            {
                payload.flags |= TestPayloadFlag_ResultsMatch;
            }
        }
    }

    payload.leak_count = global_allocation_count;

    return(payload);
}

internal_function
void test_ft_putchar_fd(Tester *tester)
{
    TestParameters tests[] =
    {
        { .char_ptr = {__LINE__, 'A',   "A"} },
        { .char_ptr = {__LINE__, 'z',   "z"} },
        { .char_ptr = {__LINE__, '\n', "\n"} },
        { .char_ptr = {__LINE__, ' ',   " "} },
        { .char_ptr = {__LINE__, '~',   "~"} },
    };
    U64 test_count = CountOfStaticArray(tests);

    // Save the current position in Tester's permanent arena and reset it only if tests were skipped.
    TemporaryArena temporary_arena = temporary_arena_begin(tester->permanent_arena);

    TestGroup test_group =
    {
        .name                      = String8Literal("ft_putchar_fd"),
        .file                      = string8_from_cstring(__FILE__),
        .failed_test_reports       = push_array(tester->permanent_arena, TestReport, test_count),
    };
    TestContext test_context =
    {
        .test_group     = test_group,

        .function_return_type     = TestReturnType_Void,
        .function_parameters_type = TestParametersType_Char,

        .tests          = tests,
        .test_count     = test_count,

        .libft_function = (void *)ft_putchar_fd,
        .callback       = (TestCallbackFunction)callback_for_putchar_fd,
    };

    run_tests(tester, &test_context);

    if(test_context.flags & TestContextFlag_TestsWereSkipped)
    {
        temporary_arena_end(temporary_arena);
    }
}
