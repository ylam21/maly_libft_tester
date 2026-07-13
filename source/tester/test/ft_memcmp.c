internal TestPayload callback_for_memcmp(TestParameters test_parameters)
{
    TestPayload payload = {0};

    TestParameters_PtrPtrSize parameters = test_parameters.ptr_ptr_size;

    // Call libc function.
    int expected_return = memcmp((const void *)parameters.ptr1, (const void *)parameters.ptr2, (size_t)parameters.size);

    // Reset global_allocation_count before calling libft function.
    global_allocation_count = 0;

    // Call libft function.
    int got_return = ft_memcmp((const void *)parameters.ptr1, (const void *)parameters.ptr2, (size_t)parameters.size);

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

internal void test_ft_memcmp(Tester *tester)
{
    U64 memory_block_1_size = TESTER_MAXIMUM_PAYLOAD_BUFFER_SIZE;
    U64 memory_block_2_size = TESTER_MAXIMUM_PAYLOAD_BUFFER_SIZE;
    U8 *memory_block_1      = push_array(tester->testing_arena, U8, memory_block_1_size);
    U8 *memory_block_2      = push_array(tester->testing_arena, U8, memory_block_2_size);
    memory_set_u32(memory_block_1, 0xDEADBEEF, memory_block_1_size);
    memory_set_u32(memory_block_2, 0xDEADBEEF, memory_block_2_size);

    // Make single byte difference between memory blocks
    memory_block_2[100] = 0x00;

    TestParameters tests[] =
    {
        // Note: Standard memcmp evaluates bytes as `unsigned char`.

        // 1. Zero-Length / Same Pointers (Should always return 0)
        { .ptr_ptr_size = {__LINE__, (void *)"Hello", (void *)"World", 0} },
        { .ptr_ptr_size = {__LINE__, memory_block_1, memory_block_2,   0} },
        { .ptr_ptr_size = {__LINE__, memory_block_1, memory_block_1, 128} },

        // 2. Standard String Comparisons
        { .ptr_ptr_size = {__LINE__, (void *)"Hello World", (void *)"Hello World", 11} },
        { .ptr_ptr_size = {__LINE__, (void *)"Hello World", (void *)"Hello Zorld", 11} },
        { .ptr_ptr_size = {__LINE__, (void *)"Hello Zorld", (void *)"Hello World", 11} },

        // 3. The Unsigned Char Trap
        { .ptr_ptr_size = {__LINE__, (void *)"\xff\xaa\xbb", (void *)"\x00\xaa\xbb", 3} },
        { .ptr_ptr_size = {__LINE__, (void *)"\x00\xaa\xbb", (void *)"\xff\xaa\xbb", 3} },

        // 4. Null Terminator Ignore Traps
        { .ptr_ptr_size = {__LINE__, (void *)"a\0b\0c", (void *)"a\0b\0c", 5} },
        { .ptr_ptr_size = {__LINE__, (void *)"a\0b\0c", (void *)"a\0b\0x", 5} },
        { .ptr_ptr_size = {__LINE__, (void *)"a\0b\0c", (void *)"a\0b\0x", 3} },

        // 5. Early Termination Trap (Bounds checking)
        { .ptr_ptr_size = {__LINE__, (void *)"42 Prague!!!", (void *)"42 Prague???", 9} }, // Difference is at index 9, size is 9, so it shouldn't be read!

        // 6. Raw Memory Blocks (Using the DEADBEEF arrays)
        { .ptr_ptr_size = {__LINE__, memory_block_1, memory_block_2, 100} }, // Difference is at 100, so checking 100 bytes should match perfectly (0 to 99)
        { .ptr_ptr_size = {__LINE__, memory_block_1, memory_block_2, 101} }, // Checking 101 bytes includes the difference, should NOT match
    };
    U64 test_count = CountOfStaticArray(tests);

    // Save the current position in Tester's permanent arena and reset it only if tests were skipped.
    TemporaryArena temporary_arena = temporary_arena_begin(tester->permanent_arena);

    TestGroup test_group =
    {
        .name                      = String8Literal("ft_memcmp"),
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

        .libft_function = (void *)ft_memcmp,
        .callback       = (TestCallbackFunction)callback_for_memcmp,
    };

    run_tests(tester, &test_context);

    if(test_context.flags & TestContextFlag_TestsWereSkipped)
    {
        temporary_arena_end(temporary_arena);
    }
}
