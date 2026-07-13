internal TestPayload callback_for_strdup(TestParameters test_parameters)
{
    TestPayload payload = {0};

    TestParameters_Ptr parameters = test_parameters.ptr;

    // Call libc function and save the result.
    char *expected_return = strdup((const char *)parameters.ptr); // Note: Null-terminated string is expected.
    U64 expected_return_length;
    if(expected_return != 0)
    {
        expected_return_length = length_of_cstring(expected_return) + 1;
        MemoryCopy(payload.buffer_for_expected, expected_return, expected_return_length);
        payload.expected_strings_sizes[0]    = expected_return_length;
        payload.expected_strings_sizes_count = 1;

        free(expected_return);
    }

    // Reset global_allocation_count before calling libft function.
    global_allocation_count = 0;

    // Call libft function and save the result.
    char *got_return = ft_strdup((const char *)parameters.ptr);
    if(got_return != 0)
    {
        U64 got_return_length = length_of_cstring(got_return) + 1;
        MemoryCopy(payload.buffer_for_got, got_return, got_return_length);
        payload.got_strings_sizes[0]    = got_return_length;
        payload.got_strings_sizes_count = 1;

        if((expected_return != 0) && (expected_return_length == got_return_length) && MemoryMatch(payload.buffer_for_got, payload.buffer_for_expected, expected_return_length))
        {
            payload.flags |= TestPayloadFlag_ResultsMatch;
        }

        free(got_return);
    }
    else
    {
        if(expected_return == 0) // This should never be the case, but if it is, handle it.
        {
            payload.flags |= TestPayloadFlag_ResultsMatch;
        }
    }

    payload.leak_count     = global_allocation_count;
    payload.expected_value = (U64)expected_return;
    payload.got_value      = (U64)got_return;

    return(payload);
}

internal void test_ft_strdup(Tester *tester)
{
    TestParameters tests[] =
    {
        // 1. Empty and Single Characters (Tests minimal malloc boundaries)
        { .ptr = {__LINE__, ""} },
        { .ptr = {__LINE__, "a"} },
        { .ptr = {__LINE__, "\n"} },

        // 2. Standard strings
        { .ptr = {__LINE__, "Hello, World!"} },
        { .ptr = {__LINE__, "42 Prague"} },
        { .ptr = {__LINE__, "0123456789"} },

        // 3. Embedded nulls (strdup MUST stop copying at the first \0)
        { .ptr = {__LINE__, "Hidden\0Secret"} },
        { .ptr = {__LINE__, "\0StartNull"} },

        // 4. Escape sequences and whitespace
        { .ptr = {__LINE__, " \t\n\v\f\r "} },
        { .ptr = {__LINE__, "\\\\Double Backslash\\\\"} },
        { .ptr = {__LINE__, "\033[31mRed Alert\033[0m"} },

        // 5. Extended ASCII / UTF-8 bytes
        { .ptr = {__LINE__, "©®™"} },
        { .ptr = {__LINE__, "\xff\xaa\x01\x02"} },

        // 6. Long string (Tests larger heap allocations)
        { .ptr = {__LINE__, "This is a significantly longer string designed to test if the student's malloc logic correctly handles sizes beyond small, single-word buffers. It needs to be long enough to expose potential off-by-one errors when iterating."} }
    };
    U64 test_count = CountOfStaticArray(tests);

    // Save the current position in Tester's permanent arena and reset it only if tests were skipped.
    TemporaryArena temporary_arena = temporary_arena_begin(tester->permanent_arena);

    TestGroup test_group =
    {
        .name                      = String8Literal("ft_strdup"),
        .file                      = string8_from_cstring(__FILE__),
        .failed_test_reports       = push_array(tester->permanent_arena, TestReport, test_count),
    };
    TestContext test_context =
    {
        .test_group     = test_group,

        .function_return_type     = TestReturnType_Ptr,
        .function_parameters_type = TestParametersType_Ptr,

        .tests          = tests,
        .test_count     = test_count,

        .libft_function = (void *)ft_strdup,
        .callback       = (TestCallbackFunction)callback_for_strdup,
    };

    run_tests(tester, &test_context);

    if(test_context.flags & TestContextFlag_TestsWereSkipped)
    {
        temporary_arena_end(temporary_arena);
    }
}
