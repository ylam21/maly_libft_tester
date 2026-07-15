internal_function
TestPayload callback_for_strchr(TestParameters test_parameters)
{
    TestPayload payload = {0};

    TestParameters_PtrInt parameters = test_parameters.ptr_int;

    // Call libc function and save the result.
    char *expected_return = strchr((const char *)parameters.ptr, (int)parameters.a);
    U64 expected_return_size;
    if(expected_return != 0)
    {
        expected_return_size = length_of_cstring(expected_return) + 1;
        MemoryCopy(payload.buffer_for_expected, expected_return, expected_return_size);
        payload.expected_strings_sizes[0]    = expected_return_size;
        payload.expected_strings_sizes_count = 1;
    }

    // Reset global_allocation_count before calling libft function.
    global_allocation_count = 0;

    // Call libft function and save the result.
    char *got_return = ft_strchr((const char *)parameters.ptr, (int)parameters.a);
    U64 got_return_size;
    if(got_return != 0)
    {
        got_return_size = length_of_cstring(got_return) + 1;
        MemoryCopy(payload.buffer_for_got, got_return, got_return_size);
        payload.got_strings_sizes[0]    = got_return_size;
        payload.got_strings_sizes_count = 1;
    }

    if(got_return == expected_return)
    {
        payload.flags |= TestPayloadFlag_ResultsMatch;
    }

    payload.leak_count     = global_allocation_count;
    payload.expected_value = (U64)expected_return;
    payload.got_value      = (U64)got_return;

    return(payload);
}

internal_function
void test_ft_strchr(Tester *tester)
{
    TestParameters tests[] =
    {
        // 1. Basic Finds (First, Middle, Last)
        { .ptr_int = {__LINE__, "Hello World", 'H'} },
        { .ptr_int = {__LINE__, "Hello World", 'W'} },
        { .ptr_int = {__LINE__, "Hello World", 'd'} },

        // 2. Multiple occurrences (Must return the FIRST occurrence)
        { .ptr_int = {__LINE__, "lorem ipsum dolor", 'o'} },
        { .ptr_int = {__LINE__, "abababab", 'b'} },

        // 3. Character not found (Must return NULL)
        { .ptr_int = {__LINE__, "Hello World", 'z'} },
        { .ptr_int = {__LINE__, "42 Prague", 'x'} },

        // 4. The Null Terminator Trap (CRITICAL)
        // strchr("string", '\0') must return a pointer to the '\0', not NULL.
        { .ptr_int = {__LINE__, "Hello World", '\0'} },

        // 5. Empty string edge cases
        { .ptr_int = {__LINE__, "", '\0'} }, // Should return pointer to the '\0'
        { .ptr_int = {__LINE__, "", 'a'} },  // Should return NULL

        // 6. Integer casting traps
        // The 'int c' parameter is internally cast to a char.
        // 'H' (72) + 256 = 328. When cast to char, it overflows back to 72.
        { .ptr_int = {__LINE__, "Hello", 'H' + 256} },
        { .ptr_int = {__LINE__, "Hello", 'e' - 256} },

        // 7. Non-standard characters
        { .ptr_int = {__LINE__, "lorem\xffipsum", '\xff'} },
    };
    U64 test_count = CountOfStaticArray(tests);

    // Save the current position in Tester's permanent arena and reset it only if tests were skipped.
    TemporaryArena temporary_arena = temporary_arena_begin(tester->permanent_arena);

    TestGroup test_group =
    {
        .name                      = String8Literal("ft_strchr"),
        .file                      = string8_from_cstring(__FILE__),
        .failed_test_reports       = push_array(tester->permanent_arena, TestReport, test_count),
    };
    TestContext test_context =
    {
        .test_group     = test_group,

        .function_return_type     = TestReturnType_Ptr,
        .function_parameters_type = TestParametersType_PtrInt,

        .tests          = tests,
        .test_count     = test_count,

        .libft_function = (void *)ft_strchr,
        .callback       = (TestCallbackFunction)callback_for_strchr,
    };

    run_tests(tester, &test_context);

    if(test_context.flags & TestContextFlag_TestsWereSkipped)
    {
        temporary_arena_end(temporary_arena);
    }
}
