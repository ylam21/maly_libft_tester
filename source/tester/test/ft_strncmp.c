internal TestPayload callback_for_strncmp(TestParameters test_parameters)
{
    TestPayload payload = {0};

    TestParameters_PtrPtrSize parameters = test_parameters.ptr_ptr_size;

    // Call libc function.
    int expected_return = strncmp((const char *)parameters.ptr1, (const char *)parameters.ptr2, (size_t)parameters.size);

    // Reset global_allocation_count before calling libft function.
    global_allocation_count = 0;

    // Call libft function.
    int got_return = ft_strncmp((const char *)parameters.ptr1, (const char *)parameters.ptr2, (size_t)parameters.size);

    U32 signs_match = 0;
    if (got_return == expected_return)
    {
        signs_match = 1;
    }
    else if ((got_return < 0 && expected_return < 0) || (got_return > 0 && expected_return > 0))
    {
        signs_match = 1;
    }

    if(signs_match)
    {
        payload.flags |= TestPayloadFlag_ResultsMatch;
    }

    payload.leak_count     = global_allocation_count;
    payload.expected_value = (U64)expected_return;
    payload.got_value      = (U64)got_return;

    return(payload);
}

internal void test_ft_strncmp(Tester *tester)
{
    TestParameters tests[] =
    {
        // 1. Zero-Length (Should always return 0, no matter what)
        { .ptr_ptr_size = {__LINE__, (void *)"Hello", (void *)"World", 0} },
        { .ptr_ptr_size = {__LINE__, (void *)"",      (void *)"",      0} },

        // 2. Exact Matches
        { .ptr_ptr_size = {__LINE__, (void *)"Hello", (void *)"Hello", 5} },
        { .ptr_ptr_size = {__LINE__, (void *)"Hello", (void *)"Hello", 10} }, // n > length

        // 3. Early Termination Trap (Difference is AFTER n)
        // Difference at index 5, but we only check up to 5 bytes (indices 0-4). Must return 0.
        { .ptr_ptr_size = {__LINE__, (void *)"Hello World", (void *)"Hello Zorld", 5} },

        // 4. Standard Differences (Checking which string is greater/lesser)
        { .ptr_ptr_size = {__LINE__, (void *)"Hello World", (void *)"Hello Zorld", 6} },
        { .ptr_ptr_size = {__LINE__, (void *)"Hello Zorld", (void *)"Hello World", 6} },

        // 5. Length Differences (One string ends earlier than the other)
        { .ptr_ptr_size = {__LINE__, (void *)"Test",  (void *)"Testing", 10} },
        { .ptr_ptr_size = {__LINE__, (void *)"Testing", (void *)"Test",  10} },

        // 6. The Null Terminator Trap (CRITICAL)
        // strncmp MUST stop at the first '\0'. The bytes after it do not matter!
        { .ptr_ptr_size = {__LINE__, (void *)"a\0b\0c", (void *)"a\0b\0x", 5} }, // Returns 0
        { .ptr_ptr_size = {__LINE__, (void *)"test\0x", (void *)"test\0y", 10} }, // Returns 0

        // 7. Empty Strings
        { .ptr_ptr_size = {__LINE__, (void *)"", (void *)"a", 1} },
        { .ptr_ptr_size = {__LINE__, (void *)"a", (void *)"", 1} },

        // 8. The Unsigned Char Trap (CRITICAL)
        // Characters must be cast to unsigned char before comparing.
        // \x80 is 128 (unsigned), but -128 (signed char).
        // If not cast to unsigned, the math goes backwards!
        { .ptr_ptr_size = {__LINE__, (void *)"\x80", (void *)"\x00", 1} },
        { .ptr_ptr_size = {__LINE__, (void *)"\x00", (void *)"\x80", 1} },
        { .ptr_ptr_size = {__LINE__, (void *)"\xff\xaa", (void *)"\x00\xaa", 2} },
    };
    U64 test_count = CountOfStaticArray(tests);

    // Save the current position in Tester's permanent arena and reset it only if tests were skipped.
    TemporaryArena temporary_arena = temporary_arena_begin(tester->permanent_arena);

    TestGroup test_group =
    {
        .name                      = String8Literal("ft_strncmp"),
        .file                      = string8_from_cstring(__FILE__),
        .failed_test_reports       = push_array(tester->permanent_arena, TestReport, test_count),
    };
    TestContext test_context =
    {
        .test_group     = test_group,

        .function_return_type     = TestReturnType_Int,
        .function_parameters_type = TestParametersType_PtrPtrSize,

        .tests          = tests,
        .test_count     = test_count,

        .libft_function = (void *)ft_strncmp,
        .callback       = (TestCallbackFunction)callback_for_strncmp,
    };

    run_tests(tester, &test_context);

    if(test_context.flags & TestContextFlag_TestsWereSkipped)
    {
        temporary_arena_end(temporary_arena);
    }
}
