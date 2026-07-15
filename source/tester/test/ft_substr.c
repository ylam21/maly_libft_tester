internal_function
TestPayload callback_for_substr(TestParameters test_parameters)
{
    TestPayload payload = {0};

    TestParameters_PtrSizeSizePtr parameters = test_parameters.ptr_size_size_ptr;

    char *expected_return = (char *)parameters.ptr2;
    U64 expected_return_length;
    if(expected_return != 0)
    {
        expected_return_length = length_of_cstring(expected_return) + 1;
        MemoryCopy(payload.buffer_for_expected, expected_return, expected_return_length);
        payload.expected_strings_sizes[0]    = expected_return_length;
        payload.expected_strings_sizes_count = 1;
    }

    // Reset global_allocation_count before calling libft function.
    global_allocation_count = 0;

    // Call libft function and save the result.
    char *got_return = ft_substr((const char *)parameters.ptr1, (unsigned int)parameters.size1, (size_t)parameters.size2);
    U64 got_return_length;
    if(got_return != 0)
    {
        got_return_length = length_of_cstring(got_return) + 1;
        MemoryCopy(payload.buffer_for_got, got_return, got_return_length);
        payload.got_strings_sizes[0]    = got_return_length;
        payload.got_strings_sizes_count = 1;

        if((expected_return != 0) &&
           (expected_return_length == got_return_length) &&
           (MemoryMatch(payload.buffer_for_got, payload.buffer_for_expected, expected_return_length)))
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

    return(payload);
}

internal_function
void test_ft_substr(Tester *tester)
{
    TestParameters tests[] =
    {
        // 1. Standard extractions
        { .ptr_size_size_ptr = {__LINE__, "Hello World", 0, 5, "Hello"} },
        { .ptr_size_size_ptr = {__LINE__, "Hello World", 6, 5, "World"} },
        { .ptr_size_size_ptr = {__LINE__, "Hello World", 2, 4, "llo "} },

        // 2. Len exceeds the remaining characters (CRITICAL: malloc bounds)
        // Students often incorrectly malloc(len + 1) instead of malloc(remaining + 1)
        { .ptr_size_size_ptr = {__LINE__, "Hello World", 6, 100, "World"} },
        { .ptr_size_size_ptr = {__LINE__, "Hello",       0, 42,  "Hello"} },

        // 3. Start index out of bounds (CRITICAL: Must return valid empty string)
        { .ptr_size_size_ptr = {__LINE__, "Hello",       5,  2, ""} }, // Exactly at '\0'
        { .ptr_size_size_ptr = {__LINE__, "Hello",       10, 5, ""} }, // Way past '\0'
        { .ptr_size_size_ptr = {__LINE__, "Hello", (unsigned int)-1, 5, ""} }, // UINT_MAX start

        // 4. Zero length requests
        { .ptr_size_size_ptr = {__LINE__, "Hello",       2,  0, ""} },
        { .ptr_size_size_ptr = {__LINE__, "Hello",       10, 0, ""} },

        // 5. Empty string inputs
        { .ptr_size_size_ptr = {__LINE__, "",            0,  0, ""} },
        { .ptr_size_size_ptr = {__LINE__, "",            1,  1, ""} },

        // 6. Maximum len requests (Tests if they guard against integer overflows)
        // (size_t)-1 evaluates to SIZE_MAX (e.g., 18,446,744,073,709,551,615)
        { .ptr_size_size_ptr = {__LINE__, "42 Prague",   0,  (size_t)-1, "42 Prague"} },
        { .ptr_size_size_ptr = {__LINE__, "42 Prague",   3,  (size_t)-1, "Prague"} },
        { .ptr_size_size_ptr = {__LINE__, "42 Prague",   99, (size_t)-1, ""} }, // Huge len, but start is out of bounds
    };
    U64 test_count = CountOfStaticArray(tests);

    // Save the current position in Tester's permanent arena and reset it only if tests were skipped.
    TemporaryArena temporary_arena = temporary_arena_begin(tester->permanent_arena);

    TestGroup test_group =
    {
        .name                      = String8Literal("ft_substr"),
        .file                      = string8_from_cstring(__FILE__),
        .failed_test_reports       = push_array(tester->permanent_arena, TestReport, test_count),
    };
    TestContext test_context =
    {
        .test_group     = test_group,

        .function_return_type     = TestReturnType_Ptr,
        .function_parameters_type = TestParametersType_PtrSizeSize,

        .tests          = tests,
        .test_count     = test_count,

        .libft_function = (void *)ft_substr,
        .callback       = (TestCallbackFunction)callback_for_substr,
    };

    run_tests(tester, &test_context);

    if(test_context.flags & TestContextFlag_TestsWereSkipped)
    {
        temporary_arena_end(temporary_arena);
    }
}
