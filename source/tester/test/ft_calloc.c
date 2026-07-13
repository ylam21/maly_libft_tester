internal TestPayload callback_for_calloc(TestParameters test_parameters)
{
    TestPayload payload = {0};

    TestParameters_SizeSize parameters = test_parameters.size_size;

    U64 total_size = parameters.size1 * parameters.size2;
    U64 check_size = MinimumBetween(total_size, TESTER_MAXIMUM_PAYLOAD_BUFFER_SIZE);

    // Call libc function.
    void *expected_return = calloc((size_t)parameters.size1, (size_t)parameters.size2);
    if(expected_return != 0)
    {
        MemoryCopy(payload.buffer_for_expected, expected_return, check_size);
        payload.expected_strings_sizes[0]    = check_size;
        payload.expected_strings_sizes_count = 1;
    }

    // Reset global_allocation_count before calling libft function.
    global_allocation_count = 0;

    // Call libft function.
    void *got_return = ft_calloc((size_t)parameters.size1, (size_t)parameters.size2);
    if(got_return != 0)
    {
        MemoryCopy(payload.buffer_for_got, got_return, check_size);
        payload.got_strings_sizes[0]    = check_size;
        payload.got_strings_sizes_count = 1;

        if((expected_return != 0) && (MemoryMatch(got_return, expected_return, total_size)))
        {
            payload.flags |= TestPayloadFlag_ResultsMatch;
        }

        free(got_return);
    }
    else
    {
        if(expected_return == 0)
        {
            payload.flags |= TestPayloadFlag_ResultsMatch;
        }
    }

    payload.leak_count     = global_allocation_count;
    payload.expected_value = (U64)expected_return;
    payload.got_value      = (U64)got_return;

    free(expected_return);

    return(payload);
}

internal void test_ft_calloc(Tester *tester)
{
    TestParameters tests[] =
    {
        // 1. Standard allocations (Checks that memory is properly zeroed)
        { .size_size = {__LINE__, 1,  50} },
        { .size_size = {__LINE__, 16, 26} },
        { .size_size = {__LINE__, 10, 10} },
        { .size_size = {__LINE__, 4,  128} },

        // 2. Zero combinations
        // Behavior varies by OS, but ft_calloc MUST match local libc behavior
        { .size_size = {__LINE__, 0, 0} },
        { .size_size = {__LINE__, 8, 0} },
        { .size_size = {__LINE__, 0, 8} },

        // 3. Size 1 edge cases
        { .size_size = {__LINE__, 1, 1} },
        { .size_size = {__LINE__, 1, 0} },
        { .size_size = {__LINE__, 0, 1} },

        // 4. Large but valid allocations
        { .size_size = {__LINE__, 100, 10} },
        { .size_size = {__LINE__, 10, 100} },

        // 5. Overflow Protections (CRITICAL)
        // students MUST check if (size1 * size2) overflows a size_t.
        // (size_t)-1 translates to SIZE_MAX (e.g., 18,446,744,073,709,551,615)
        { .size_size = {__LINE__, (size_t)-1, 2} },
        { .size_size = {__LINE__, 2, (size_t)-1} },
        { .size_size = {__LINE__, (size_t)-1, (size_t)-1} },
    };
    U64 test_count = CountOfStaticArray(tests);

    // Save the current position in Tester's permanent arena and reset it only if tests were skipped.
    TemporaryArena temporary_arena = temporary_arena_begin(tester->permanent_arena);

    TestGroup test_group =
    {
        .name                      = String8Literal("ft_calloc"),
        .file                      = string8_from_cstring(__FILE__),
        .failed_test_reports       = push_array(tester->permanent_arena, TestReport, test_count),
    };
    TestContext test_context =
    {
        .test_group     = test_group,

        .function_return_type     = TestReturnType_Ptr,
        .function_parameters_type = TestParametersType_SizeSize,

        .tests          = tests,
        .test_count     = test_count,

        .libft_function = (void *)ft_calloc,
        .callback       = (TestCallbackFunction)callback_for_calloc,
    };

    run_tests(tester, &test_context);

    if(test_context.flags & TestContextFlag_TestsWereSkipped)
    {
        temporary_arena_end(temporary_arena);
    }
}
