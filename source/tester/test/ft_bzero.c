internal_function
TestPayload callback_for_bzero(TestParameters test_parameters)
{
    TestPayload payload = {0};

    TestParameters_PtrSize parameters = test_parameters.ptr_size;

    // Backup memory to backup buffer.
    U8 backup_buffer[TESTER_MAXIMUM_PAYLOAD_BUFFER_SIZE];
    MemoryCopy(backup_buffer, parameters.ptr, parameters.size);

    // Call libc function and save the result.
    bzero((void *)parameters.ptr, (size_t)parameters.size);
    MemoryCopy(payload.buffer_for_expected, parameters.ptr, parameters.size);
    payload.expected_strings_sizes[0]    = parameters.size;
    payload.expected_strings_sizes_count = 1;

    // Restore memory with backup buffer.
    MemoryCopy(parameters.ptr, backup_buffer, parameters.size);

    // Reset global_allocation_count before calling libft function.
    global_allocation_count = 0;

    // Call libft function and save the result.
    ft_bzero((void *)parameters.ptr, (size_t)parameters.size);
    MemoryCopy(payload.buffer_for_got, parameters.ptr, parameters.size);
    payload.got_strings_sizes[0]    = parameters.size;
    payload.got_strings_sizes_count = 1;

    if(MemoryMatch(payload.buffer_for_expected, payload.buffer_for_got, parameters.size))
    {
        payload.flags |= TestPayloadFlag_ResultsMatch;
    }

    payload.leak_count = global_allocation_count;

    // Restore memory with backup buffer for the next test.
    MemoryCopy(parameters.ptr, backup_buffer, parameters.size);

    return(payload);
}

internal_function
void test_ft_bzero(Tester *tester)
{
    U64 memory_block_size = TESTER_MAXIMUM_PAYLOAD_BUFFER_SIZE;
    U8 *memory_block      = push_array(tester->testing_arena, U8, memory_block_size);
    memory_set_u32(memory_block, 0xDEADBEEF, memory_block_size);

    TestParameters tests[] =
    {
        // Note: Passing NULL pointer to bzero results in undefined behaviour. Do not write such tests.
        // Note: Make sure we do not touch any memory out of bounds this memory block.

        // 1. Basic Standard Clears
        { .ptr_size = {__LINE__, memory_block,  10} },
        { .ptr_size = {__LINE__, memory_block,  42} },
        { .ptr_size = {__LINE__, memory_block, 100} },

        // 2. Zero-length
        { .ptr_size = {__LINE__, memory_block,      0} },
        { .ptr_size = {__LINE__, memory_block + 50, 0} },

        // 3. Single byte clears
        { .ptr_size = {__LINE__, memory_block, 1}     },
        { .ptr_size = {__LINE__, memory_block + 7, 1} },

        // 4. Unaligned Addresses
        { .ptr_size = {__LINE__, memory_block + 1, 42} },
        { .ptr_size = {__LINE__, memory_block + 3, 42} },
        { .ptr_size = {__LINE__, memory_block + 7, 42} },

        // 5. Unaligned Sizes
        { .ptr_size = {__LINE__, memory_block,  7} },
        { .ptr_size = {__LINE__, memory_block, 15} },
        { .ptr_size = {__LINE__, memory_block, 33} },

        // 6. Unaligned Address + Unaligned Size
        { .ptr_size = {__LINE__, memory_block + 5,  37} },
        { .ptr_size = {__LINE__, memory_block + 11, 89} },

        // 7. Maximum valid boundary clears
        { .ptr_size = {__LINE__, memory_block,       memory_block_size      } },
        { .ptr_size = {__LINE__, memory_block + 1,   memory_block_size -   1} },
        { .ptr_size = {__LINE__, memory_block + 127, memory_block_size - 127} },
    };
    U64 test_count = CountOfStaticArray(tests);

    // Save the current position in Tester's permanent arena and reset it only if tests were skipped.
    TemporaryArena temporary_arena = temporary_arena_begin(tester->permanent_arena);

    TestGroup test_group =
    {
        .name                      = String8Literal("ft_bzero"),
        .file                      = string8_from_cstring(__FILE__),
        .failed_test_reports       = push_array(tester->permanent_arena, TestReport, test_count),
    };
    TestContext test_context =
    {
        .test_group     = test_group,

        .function_return_type     = TestReturnType_Void,
        .function_parameters_type = TestParametersType_PtrSize,

        .tests          = tests,
        .test_count     = test_count,

        .libft_function = (void *)ft_bzero,
        .callback       = (TestCallbackFunction)callback_for_bzero,
    };

    run_tests(tester, &test_context);

    if(test_context.flags & TestContextFlag_TestsWereSkipped)
    {
        temporary_arena_end(temporary_arena);
    }
}
