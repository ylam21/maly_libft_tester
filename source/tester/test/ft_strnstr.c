internal_function
TestPayload callback_for_strnstr(TestParameters test_parameters)
{
    TestPayload payload = {0};

    TestParameters_PtrPtrSize parameters = test_parameters.ptr_ptr_size;

    // Call libc function and save the result.
    char *expected_return = strnstr((const char *)parameters.ptr1, (const char *)parameters.ptr2, (size_t)parameters.size);
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
    char *got_return = ft_strnstr((const char *)parameters.ptr1, (const char *)parameters.ptr2, (size_t)parameters.size);
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
void test_ft_strnstr(Tester *tester)
{
    TestParameters tests[] =
    {
        // 1. Basic Finds
        { .ptr_ptr_size = {__LINE__, (void *)"Hello World", (void *)"World", 15} },
        { .ptr_ptr_size = {__LINE__, (void *)"Hello World", (void *)"Hello", 15} },

        // 2. The Empty Needle Trap (CRITICAL)
        // If needle is empty, return haystack immediately, even if len is 0.
        { .ptr_ptr_size = {__LINE__, (void *)"Hello World", (void *)"",      15} },
        { .ptr_ptr_size = {__LINE__, (void *)"Hello World", (void *)"",      0} },

        // 3. Exact Boundaries
        // "World" starts at index 6. Length of "World" is 5. 6 + 5 = 11.
        { .ptr_ptr_size = {__LINE__, (void *)"Hello World", (void *)"World", 11} }, // Fits exactly
        { .ptr_ptr_size = {__LINE__, (void *)"Hello World", (void *)"World", 10} }, // Needle goes past len -> NULL

        // 4. Zero Length (Should return NULL unless needle is empty)
        { .ptr_ptr_size = {__LINE__, (void *)"Hello",       (void *)"H",     0} },

        // 5. Partial Matches / Repeated Sequences
        { .ptr_ptr_size = {__LINE__, (void *)"aabaabaa",    (void *)"aab",   5} }, // Finds the first one
        { .ptr_ptr_size = {__LINE__, (void *)"lorem ipsum", (void *)"ipsum", 9} }, // "ips" is within len, but "um" is not -> NULL

        // 6. Not Found
        { .ptr_ptr_size = {__LINE__, (void *)"Hello",       (void *)"x",     5} },
        { .ptr_ptr_size = {__LINE__, (void *)"Hello",       (void *)"HelloX",10} }, // Needle larger than haystack

        // 7. Haystack Null-Terminator Check
        // Must stop searching at \0 even if len is larger!
        { .ptr_ptr_size = {__LINE__, (void *)"Hi\0There",   (void *)"There", 10} }, // Returns NULL

        // 8. Empty Haystack
        { .ptr_ptr_size = {__LINE__, (void *)"",            (void *)"a",     10} },
        { .ptr_ptr_size = {__LINE__, (void *)"",            (void *)"",      10} },
    };
    U64 test_count = CountOfStaticArray(tests);

    // Save the current position in Tester's permanent arena and reset it only if tests were skipped.
    TemporaryArena temporary_arena = temporary_arena_begin(tester->permanent_arena);

    TestGroup test_group =
    {
        .name                      = String8Literal("ft_strnstr"),
        .file                      = string8_from_cstring(__FILE__),
        .failed_test_reports       = push_array(tester->permanent_arena, TestReport, test_count),
    };
    TestContext test_context =
    {
        .test_group     = test_group,

        .function_return_type     = TestReturnType_Ptr,
        .function_parameters_type = TestParametersType_PtrPtrSize,

        .tests          = tests,
        .test_count     = test_count,

        .libft_function = (void *)ft_strnstr,
        .callback       = (TestCallbackFunction)callback_for_strnstr,
    };

    run_tests(tester, &test_context);

    if(test_context.flags & TestContextFlag_TestsWereSkipped)
    {
        temporary_arena_end(temporary_arena);
    }
}
