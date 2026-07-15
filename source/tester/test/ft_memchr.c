internal_function
TestPayload callback_for_memchr(TestParameters test_parameters)
{
    TestPayload payload = {0};

    TestParameters_PtrIntSize parameters = test_parameters.ptr_int_size;

    // Call libc function and save the result.
    void *expected_return = memchr((const void *)parameters.ptr, (int)parameters.a,(size_t)parameters.size);
    if(expected_return != 0)
    {
        U64 remaining_size = (U64)((U8*)parameters.ptr + parameters.size - (U8*)expected_return);
        MemoryCopy(payload.buffer_for_expected, expected_return, remaining_size);
        payload.expected_strings_sizes[0]    = remaining_size;
        payload.expected_strings_sizes_count = 1;
    }

    // Reset global_allocation_count before calling libft function.
    global_allocation_count = 0;

    // Call libft function and save the result.
    void *got_return = ft_memchr((const void *)parameters.ptr, (int)parameters.a,(size_t)parameters.size);
    if(got_return != 0)
    {
        U64 remaining_size = (U64)((U8*)parameters.ptr + parameters.size - (U8*)got_return);
        MemoryCopy(payload.buffer_for_got, got_return, remaining_size);
        payload.got_strings_sizes[0]    = remaining_size;
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
void test_ft_memchr(Tester *tester)
{
    TestParameters tests[] =
    {
        // Note: Passing NULL pointer to memchr results in undefined behaviour. Do not write such tests.
        // Note: memchr MUST cast the 'int' to 'unsigned char' internally.
        //       If ft_memchr compares using (char) some tests will be resolved as failed.

        // 1. Basic Standard Finds
        { .ptr_int_size = {__LINE__, "Hello", 'H',  6} },
        { .ptr_int_size = {__LINE__, "Hello", 'l',  6} },
        { .ptr_int_size = {__LINE__, "Hello", 'o',  6} },
        { .ptr_int_size = {__LINE__, "Hello", 'z',  6} },
        { .ptr_int_size = {__LINE__, "Hello", 'o',  2} },
        { .ptr_int_size = {__LINE__, "Hello", 'H',  0} },
        { .ptr_int_size = {__LINE__, "Hello", '\0', 6} },
        { .ptr_int_size = {__LINE__, "Hello", '\0', 5} },

        // 2. Casting Traps
        { .ptr_int_size = {__LINE__, "Find \xff the trap", -1,   15} }, // -1 cast to unsigned char is 255 (\xff)
        { .ptr_int_size = {__LINE__, "Find \xff the trap", 255,  15} }, // Same as above, checking positive overflow
        { .ptr_int_size = {__LINE__, "Find \x80 the trap", -128, 15} }, // -128 cast to unsigned char is 128 (\x80)

        // 3. Integer Overflow Casting Trap
        { .ptr_int_size = {__LINE__, "Hello", 'H' + 256, 6} }, // 'H' + 256 = 328. Cast to unsigned char, it wraps back to 72 ('H').

        // 4. Boundary & Off-By-One Traps
        { .ptr_int_size = {__LINE__, "42Prague", 'e', 8} }, // Target is the exact last byte allowed
        { .ptr_int_size = {__LINE__, "42Prague", 'e', 7} }, // Target is exactly one byte out of bounds (Should return NULL)
        { .ptr_int_size = {__LINE__, "42Prague", 'P', 1} }, // Size is 1, target is at index 2 (Should return NULL)

        // 5. Raw Memory Traps (Ignoring Null Terminators)
        { .ptr_int_size = {__LINE__, "a\0b\0c\0d", 'c', 8} }, // Must search past the first two null bytes
        { .ptr_int_size = {__LINE__, "a\0b\0c\0d", 'd', 8} }, // Must search past three null bytes
        { .ptr_int_size = {__LINE__, "a\0b\0c\0d", 'x', 8} }, // Should search the whole buffer and return NULL

        // 6. Multiple Occurrences
        { .ptr_int_size = {__LINE__, "mississippi", 's', 12} },
        { .ptr_int_size = {__LINE__, "mississippi", 'p', 12} },
    };
    U64 test_count = CountOfStaticArray(tests);

    // Save the current position in Tester's permanent arena and reset it only if tests were skipped.
    TemporaryArena temporary_arena = temporary_arena_begin(tester->permanent_arena);

    TestGroup test_group =
    {
        .name                      = String8Literal("ft_memchr"),
        .file                      = string8_from_cstring(__FILE__),
        .failed_test_reports       = push_array(tester->permanent_arena, TestReport, test_count),
    };
    TestContext test_context =
    {
        .test_group     = test_group,

        .function_return_type     = TestReturnType_Ptr,
        .function_parameters_type = TestParametersType_PtrIntSize,

        .tests          = tests,
        .test_count     = test_count,

        .libft_function = (void *)ft_memchr,
        .callback       = (TestCallbackFunction)callback_for_memchr,
    };

    run_tests(tester, &test_context);

    if(test_context.flags & TestContextFlag_TestsWereSkipped)
    {
        temporary_arena_end(temporary_arena);
    }
}
