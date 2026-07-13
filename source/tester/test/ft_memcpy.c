internal TestPayload callback_for_memcpy(TestParameters test_parameters)
{
    TestPayload payload = {0};

    TestParameters_PtrPtrSize parameters = test_parameters.ptr_ptr_size;

    // Backup memory to backup buffer.
    U8 backup_buffer[TESTER_MAXIMUM_PAYLOAD_BUFFER_SIZE];
    MemoryCopy(backup_buffer, parameters.ptr1, parameters.size);

    // Call libc function and save the result.
    void *expected_return = memcpy((void *)parameters.ptr1, (const void *)parameters.ptr2, (size_t)parameters.size);
    if(expected_return != 0)
    {
        MemoryCopy(payload.buffer_for_expected, expected_return, parameters.size);
        payload.expected_strings_sizes[0]    = parameters.size;
        payload.expected_strings_sizes_count = 1;
    }

    // Restore memory with backup buffer.
    MemoryCopy(parameters.ptr1, backup_buffer, parameters.size);

    // Reset global_allocation_count before calling libft function.
    global_allocation_count = 0;

    // Call libft function and save the result.
    void *got_return = ft_memcpy((void *)parameters.ptr1, (const void *)parameters.ptr2, (size_t)parameters.size);
    if(got_return != 0)
    {
        MemoryCopy(payload.buffer_for_got, got_return, parameters.size);
        payload.got_strings_sizes[0]    = parameters.size;
        payload.got_strings_sizes_count = 1;

        if(got_return == expected_return && MemoryMatch(payload.buffer_for_expected, payload.buffer_for_got, parameters.size))
        {
            payload.flags |= TestPayloadFlag_ResultsMatch;
        }
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

    // Restore memory with backup buffer for the next test.
    MemoryCopy(parameters.ptr1, backup_buffer, parameters.size);

    return(payload);
}

internal void test_ft_memcpy(Tester *tester)
{
    U64 memory_block_1_size = TESTER_MAXIMUM_PAYLOAD_BUFFER_SIZE;
    U64 memory_block_2_size = TESTER_MAXIMUM_PAYLOAD_BUFFER_SIZE;
    U8 *memory_block_1      = push_array(tester->testing_arena, U8, memory_block_1_size);
    U8 *memory_block_2      = push_array(tester->testing_arena, U8, memory_block_2_size);
    memory_set_u32(memory_block_1, 0xDEADBEEF, memory_block_1_size);
    memory_set_u32(memory_block_2, 0x12345678, memory_block_2_size);

    TestParameters tests[] =
    {
        // Note: "...The memory areas must not overlap. Use memmove(3) if the memory areas do overlap." (https://man7.org/linux/man-pages/man3/memcpy.3.html)
        //        Do not write tests where memory areas overlap.

        // 1. Basic Copies
        { .ptr_ptr_size = {__LINE__, memory_block_1, memory_block_2,          10} },
        { .ptr_ptr_size = {__LINE__, memory_block_1, memory_block_2,          128} },
        { .ptr_ptr_size = {__LINE__, memory_block_1, memory_block_2,          memory_block_2_size} }, // Full buffer

        // 2. Zero-Size Edge Cases (Should not crash or modify memory)
        { .ptr_ptr_size = {__LINE__, memory_block_1, memory_block_2,          0} },
        { .ptr_ptr_size = {__LINE__, memory_block_1 + 50, memory_block_2 + 50,0} },
        { .ptr_ptr_size = {__LINE__, NULL, NULL,                              0} }, // Standard protection test

        // 3. Unaligned Destination
        { .ptr_ptr_size = {__LINE__, memory_block_1 + 1, memory_block_2,      42} },
        { .ptr_ptr_size = {__LINE__, memory_block_1 + 3, memory_block_2,      15} },
        { .ptr_ptr_size = {__LINE__, memory_block_1 + 7, memory_block_2,      113} },

        // 4. Unaligned Source
        { .ptr_ptr_size = {__LINE__, memory_block_1, memory_block_2 + 1,      42} },
        { .ptr_ptr_size = {__LINE__, memory_block_1, memory_block_2 + 3,      15} },
        { .ptr_ptr_size = {__LINE__, memory_block_1, memory_block_2 + 7,      113} },

        // 5. Unaligned Source AND Destination (Worst case for word-optimizations)
        { .ptr_ptr_size = {__LINE__, memory_block_1 + 1, memory_block_2 + 3,  117} },
        { .ptr_ptr_size = {__LINE__, memory_block_1 + 3, memory_block_2 + 1,  117} },
        { .ptr_ptr_size = {__LINE__, memory_block_1 + 7, memory_block_2 + 2,  89} },
    };
    U64 test_count = CountOfStaticArray(tests);

    // Save the current position in Tester's permanent arena and reset it only if tests were skipped.
    TemporaryArena temporary_arena = temporary_arena_begin(tester->permanent_arena);

    TestGroup test_group =
    {
        .name                      = String8Literal("ft_memcpy"),
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

        .libft_function = (void *)ft_memcpy,
        .callback       = (TestCallbackFunction)callback_for_memcpy,
    };

    run_tests(tester, &test_context);

    if(test_context.flags & TestContextFlag_TestsWereSkipped)
    {
        temporary_arena_end(temporary_arena);
    }
}
