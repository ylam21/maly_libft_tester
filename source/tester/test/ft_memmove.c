internal_function
TestPayload callback_for_memmove(TestParameters test_parameters)
{
    TestPayload payload = {0};

    TestParameters_PtrPtrSize parameters = test_parameters.ptr_ptr_size;

    // Backup memory to backup buffer.
    U8 backup_buffer[TESTER_MAXIMUM_PAYLOAD_BUFFER_SIZE];
    MemoryCopy(backup_buffer, parameters.ptr1, parameters.size);

    // Call libc function and save the result.
    void *expected_return = memmove((void *)parameters.ptr1, (const void *)parameters.ptr2, (size_t)parameters.size);
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
    void *got_return = ft_memmove((void *)parameters.ptr1, (const void *)parameters.ptr2, (size_t)parameters.size);
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

internal_function
void test_ft_memmove(Tester *tester)
{
    U64 memory_block_1_size = TESTER_MAXIMUM_PAYLOAD_BUFFER_SIZE;
    U64 memory_block_2_size = TESTER_MAXIMUM_PAYLOAD_BUFFER_SIZE;
    U8 *memory_block_1      = push_array(tester->testing_arena, U8, memory_block_1_size);
    U8 *memory_block_2      = push_array(tester->testing_arena, U8, memory_block_2_size);
    memory_set_u32(memory_block_1, 0xDEADBEEF, memory_block_1_size);
    memory_set_u32(memory_block_2, 0x12345678, memory_block_2_size);

    TestParameters tests[] =
    {
        // 1. Standard Non-Overlapping (Independent Buffers)
        { .ptr_ptr_size = {__LINE__, memory_block_2, memory_block_1,          10} },
        { .ptr_ptr_size = {__LINE__, memory_block_2 + 17, memory_block_1 + 3, 33} },

        // 2. Fundamental Overlaps (Inside the same buffer)
        { .ptr_ptr_size = {__LINE__, memory_block_1 + 2, memory_block_1,       10} }, // Dest > Src (Must copy backwards)
        { .ptr_ptr_size = {__LINE__, memory_block_1, memory_block_1 + 2,       10} }, // Dest < Src (Must copy forwards)
        { .ptr_ptr_size = {__LINE__, memory_block_1 + 80, memory_block_1 + 10, 20} }, // Non-overlapping but same buffer
        { .ptr_ptr_size = {__LINE__, memory_block_1 + 10, memory_block_1 + 80, 20} }, // Non-overlapping but same buffer

        // 3. The 1-Byte Shifts
        { .ptr_ptr_size = {__LINE__, memory_block_1 + 1, memory_block_1, 127} }, // Shift right
        { .ptr_ptr_size = {__LINE__, memory_block_1, memory_block_1 + 1, 127} }, // Shift left
        { .ptr_ptr_size = {__LINE__, memory_block_1 + 1, memory_block_1, 511} }, // Huge shift right (Maximum possible size for +1 offset)
        { .ptr_ptr_size = {__LINE__, memory_block_1, memory_block_1 + 1, 511} }, // Huge shift left (Maximum possible size for +1 offset)

        // 4. Zero-Size & Same Pointer Edge Cases
        { .ptr_ptr_size = {__LINE__, memory_block_2, memory_block_1,           0} },
        { .ptr_ptr_size = {__LINE__, memory_block_1 + 50, memory_block_1 + 50, 0} },
        { .ptr_ptr_size = {__LINE__, memory_block_1, memory_block_1,          10} }, // Same pointer, small size
        { .ptr_ptr_size = {__LINE__, memory_block_1, memory_block_1,         128} }, // Same pointer, larger size

        // 5. Unaligned Memory
        { .ptr_ptr_size = {__LINE__, memory_block_1 + 3, memory_block_1 + 8, 50} }, // Dest odd, Src even
        { .ptr_ptr_size = {__LINE__, memory_block_1 + 8, memory_block_1 + 3, 50} }, // Dest even, Src odd
        { .ptr_ptr_size = {__LINE__, memory_block_1 + 10, memory_block_1 + 9, 1} }, // Tiny 1-byte overlap

        // 6. "Worst Case" Overlaps (Unaligned Src + Dest + Size)
        { .ptr_ptr_size = {__LINE__, memory_block_1 + 3, memory_block_1 + 1, 117} },
        { .ptr_ptr_size = {__LINE__, memory_block_1 + 1, memory_block_1 + 3, 117} },
        { .ptr_ptr_size = {__LINE__, memory_block_1 + 7, memory_block_1 + 2,  89} },
        { .ptr_ptr_size = {__LINE__, memory_block_1 + 2, memory_block_1 + 7,  89} },

        // 7. Perfect Seams & Maximum Boundaries
        { .ptr_ptr_size = {__LINE__, memory_block_1 + 64, memory_block_1,   64} }, // Seam: Dest starts where Src ends
        { .ptr_ptr_size = {__LINE__, memory_block_1 + 256, memory_block_1, 256} }, // Perfect half-buffer seam right (256 + 256 = 512)
        { .ptr_ptr_size = {__LINE__, memory_block_1, memory_block_1 + 256, 256} }, // Perfect half-buffer seam left
        { .ptr_ptr_size = {__LINE__, memory_block_1, memory_block_2,       512} }, // Copy exactly the entire 512-byte limit
        { .ptr_ptr_size = {__LINE__, memory_block_1 + 511, memory_block_2,   1} }, // Copy to the exact last byte of the 512 buffer (Index 511)
    };
    U64 test_count = CountOfStaticArray(tests);

    // Save the current position in Tester's permanent arena and reset it only if tests were skipped.
    TemporaryArena temporary_arena = temporary_arena_begin(tester->permanent_arena);

    TestGroup test_group =
    {
        .name                      = String8Literal("ft_memmove"),
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

        .libft_function = (void *)ft_memmove,
        .callback       = (TestCallbackFunction)callback_for_memmove,
    };

    run_tests(tester, &test_context);

    if(test_context.flags & TestContextFlag_TestsWereSkipped)
    {
        temporary_arena_end(temporary_arena);
    }
}
