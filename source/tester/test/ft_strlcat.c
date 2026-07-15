internal_function
TestPayload callback_for_strlcat(TestParameters test_parameters)
{
    TestPayload payload = {0};

    TestParameters_PtrPtrSize parameters = test_parameters.ptr_ptr_size;

    // Backup memory to backup buffer.
    U8 backup_buffer[TESTER_MAXIMUM_PAYLOAD_BUFFER_SIZE];
    MemoryCopy(backup_buffer, parameters.ptr1, TESTER_MAXIMUM_PAYLOAD_BUFFER_SIZE);

    // Call libc function and save the result.
    size_t expected_return = strlcat((void *)parameters.ptr1, (const void *)parameters.ptr2, (size_t)parameters.size);

    U64 expected_return_length = expected_return + 1;
    expected_return_length     = MinimumBetween(expected_return_length, TESTER_MAXIMUM_PAYLOAD_BUFFER_SIZE);
    MemoryCopy(payload.buffer_for_expected, parameters.ptr1, expected_return_length);
    payload.expected_strings_sizes[0]    = expected_return_length;
    payload.expected_strings_sizes_count = 1;

    // Restore memory with backup buffer.
    MemoryCopy(parameters.ptr1, backup_buffer, TESTER_MAXIMUM_PAYLOAD_BUFFER_SIZE);

    // Reset global_allocation_count before calling libft function.
    global_allocation_count = 0;

    // Call libft function and save the result.
    size_t got_return = ft_strlcat((void *)parameters.ptr1, (const void *)parameters.ptr2, (size_t)parameters.size);

    U64 got_return_length = got_return + 1;
    got_return_length     = MinimumBetween(got_return_length, TESTER_MAXIMUM_PAYLOAD_BUFFER_SIZE);
    MemoryCopy(payload.buffer_for_got, parameters.ptr1, got_return_length);
    payload.got_strings_sizes[0]    = got_return_length;
    payload.got_strings_sizes_count = 1;

    if((got_return_length == expected_return_length) && (MemoryMatch(payload.buffer_for_got, payload.buffer_for_expected, expected_return_length)))
    {
        payload.flags |= TestPayloadFlag_ResultsMatch;
    }

    payload.leak_count     = global_allocation_count;
    payload.expected_value = (U64)expected_return;
    payload.got_value      = (U64)got_return;

    // Restore memory with backup buffer for the next test.
    MemoryCopy(parameters.ptr1, backup_buffer, TESTER_MAXIMUM_PAYLOAD_BUFFER_SIZE);

    return(payload);
}

internal_function
void test_ft_strlcat(Tester *tester)
{
    U64 memory_block_size = TESTER_MAXIMUM_PAYLOAD_BUFFER_SIZE;
    U8 *memory_block      = push_array(tester->testing_arena, U8, memory_block_size);
    memory_set_u32(memory_block, 0xDEADBEEF, memory_block_size);
    String8 prefix_string = String8Literal("Hello, I am the prefix!>");
    MemoryCopy(memory_block, prefix_string.str, prefix_string.size + 1);

    TestParameters tests[] =
    {
        // Prefix length is exactly 24 bytes.

        // 1. Standard Appending (Plenty of space: dstsize > 24 + strlen(src))
        { .ptr_ptr_size = {__LINE__, memory_block, "World",       50} },
        { .ptr_ptr_size = {__LINE__, memory_block, "42",          50} },

        // 2. Exact fit (dstsize == 24 + strlen(src) + 1)
        { .ptr_ptr_size = {__LINE__, memory_block, "World",       30} }, // 24 + 5 + 1 = 30

        // 3. Normal Truncation (Appends some characters, then \0)
        // Must return 24 + strlen(src)
        { .ptr_ptr_size = {__LINE__, memory_block, "World",       28} }, // Appends "Wor\0"
        { .ptr_ptr_size = {__LINE__, memory_block, "A",           25} }, // Appends "\0" only

        // 4. The Edge of Truncation (dstsize == strlen(dst))
        // CRITICAL: Must append nothing, keep existing \0, return dstsize + strlen(src)
        { .ptr_ptr_size = {__LINE__, memory_block, "World",       24} },

        // 5. The "Unterminated" Trap (dstsize < strlen(dst))
        // CRITICAL: Must not write a null terminator at all! Returns dstsize + strlen(src)
        { .ptr_ptr_size = {__LINE__, memory_block, "World",       15} },
        { .ptr_ptr_size = {__LINE__, memory_block, "World",       2} },

        // 6. Zero dstsize (Must not touch memory at all, returns strlen(src))
        { .ptr_ptr_size = {__LINE__, memory_block, "Do not copy", 0} },
        { .ptr_ptr_size = {__LINE__, memory_block, "",            0} },

        // 7. Empty source combinations
        { .ptr_ptr_size = {__LINE__, memory_block, "",            50} }, // Plenty of space
        { .ptr_ptr_size = {__LINE__, memory_block, "",            10} }, // dstsize < strlen(dst)

        // 8. Massive dstsize (Catches students who incorrectly zero-pad)
        { .ptr_ptr_size = {__LINE__, memory_block, "Short",       TESTER_MAXIMUM_PAYLOAD_BUFFER_SIZE} },
    };
    U64 test_count = CountOfStaticArray(tests);

    // Save the current position in Tester's permanent arena and reset it only if tests were skipped.
    TemporaryArena temporary_arena = temporary_arena_begin(tester->permanent_arena);

    TestGroup test_group =
    {
        .name                      = String8Literal("ft_strlcat"),
        .file                      = string8_from_cstring(__FILE__),
        .failed_test_reports       = push_array(tester->permanent_arena, TestReport, test_count),
    };
    TestContext test_context =
    {
        .test_group     = test_group,

        .function_return_type     = TestReturnType_Size,
        .function_parameters_type = TestParametersType_PtrPtrSize,

        .tests          = tests,
        .test_count     = test_count,

        .libft_function = (void *)ft_strlcat,
        .callback       = (TestCallbackFunction)callback_for_strlcat,
    };

    run_tests(tester, &test_context);

    if(test_context.flags & TestContextFlag_TestsWereSkipped)
    {
        temporary_arena_end(temporary_arena);
    }
}
