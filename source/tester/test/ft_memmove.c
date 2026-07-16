global U8 memmove_memory_block_1[TESTER_MAXIMUM_PAYLOAD_BUFFER_SIZE];
global U8 memmove_memory_block_2[TESTER_MAXIMUM_PAYLOAD_BUFFER_SIZE];

internal_function
TestPayload callback_for_memmove(TestParameters test_parameters)
{
    TestPayload payload = {0};

    TestParameters_PtrPtrSize parameters = test_parameters.ptr_ptr_size;

    memory_set_u32(memmove_memory_block_1, 0xDEADBEEF, TESTER_MAXIMUM_PAYLOAD_BUFFER_SIZE);
    memory_set_u32(memmove_memory_block_2, 0x12345678, TESTER_MAXIMUM_PAYLOAD_BUFFER_SIZE);

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

    // Reset thread_static_allocation_count before calling libft function.
    thread_static_allocation_count = 0;

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

    if(!thread_static_allocation_count)
    {
        payload.flags |= TestPayloadFlag_NoMemoryLeak;
    }

    if((payload.flags & TestPayloadFlag_ResultsMatch) && (payload.flags & TestPayloadFlag_NoMemoryLeak))
    {
        payload.flags |= TestPayloadFlag_TestPassed;
    }

    payload.leak_count     = thread_static_allocation_count;
    payload.expected_value = (U64)expected_return;
    payload.got_value      = (U64)got_return;

    return(payload);
}

read_only global TestGroup test_group_ft_memmove =
{
    .tests =
    {
        [0]  = { .ptr_ptr_size = {memmove_memory_block_2, memmove_memory_block_1,          10} },
        [1]  = { .ptr_ptr_size = {memmove_memory_block_2 + 17, memmove_memory_block_1 + 3, 33} },
        [2]  = { .ptr_ptr_size = {memmove_memory_block_1 + 2, memmove_memory_block_1,       10} }, // Dest > Src (Must copy backwards)
        [3]  = { .ptr_ptr_size = {memmove_memory_block_1, memmove_memory_block_1 + 2,       10} }, // Dest < Src (Must copy forwards)
        [4]  = { .ptr_ptr_size = {memmove_memory_block_1 + 80, memmove_memory_block_1 + 10, 20} }, // Non-overlapping but same buffer
        [5]  = { .ptr_ptr_size = {memmove_memory_block_1 + 10, memmove_memory_block_1 + 80, 20} }, // Non-overlapping but same buffer
        [6]  = { .ptr_ptr_size = {memmove_memory_block_1 + 1, memmove_memory_block_1, 127} }, // Shift right
        [7]  = { .ptr_ptr_size = {memmove_memory_block_1, memmove_memory_block_1 + 1, 127} }, // Shift left
        [8]  = { .ptr_ptr_size = {memmove_memory_block_1 + 1, memmove_memory_block_1, 511} }, // Huge shift right (Maximum possible size for +1 offset)
        [9]  = { .ptr_ptr_size = {memmove_memory_block_1, memmove_memory_block_1 + 1, 511} }, // Huge shift left (Maximum possible size for +1 offset)
        [10] = { .ptr_ptr_size = {memmove_memory_block_2, memmove_memory_block_1,           0} },
        [11] = { .ptr_ptr_size = {memmove_memory_block_1 + 50, memmove_memory_block_1 + 50, 0} },
        [12] = { .ptr_ptr_size = {memmove_memory_block_1, memmove_memory_block_1,          10} }, // Same pointer, small size
        [13] = { .ptr_ptr_size = {memmove_memory_block_1, memmove_memory_block_1,         128} }, // Same pointer, larger size
        [14] = { .ptr_ptr_size = {memmove_memory_block_1 + 3, memmove_memory_block_1 + 8, 50} }, // Dest odd, Src even
        [15] = { .ptr_ptr_size = {memmove_memory_block_1 + 8, memmove_memory_block_1 + 3, 50} }, // Dest even, Src odd
        [16] = { .ptr_ptr_size = {memmove_memory_block_1 + 10, memmove_memory_block_1 + 9, 1} }, // Tiny 1-byte overlap
        [17] = { .ptr_ptr_size = {memmove_memory_block_1 + 3, memmove_memory_block_1 + 1, 117} },
        [18] = { .ptr_ptr_size = {memmove_memory_block_1 + 1, memmove_memory_block_1 + 3, 117} },
        [19] = { .ptr_ptr_size = {memmove_memory_block_1 + 7, memmove_memory_block_1 + 2,  89} },
        [20] = { .ptr_ptr_size = {memmove_memory_block_1 + 2, memmove_memory_block_1 + 7,  89} },
        [21] = { .ptr_ptr_size = {memmove_memory_block_1 + 64, memmove_memory_block_1,   64} }, // Seam: Dest starts where Src ends
        [22] = { .ptr_ptr_size = {memmove_memory_block_1 + 256, memmove_memory_block_1, 256} }, // Perfect half-buffer seam right (256 + 256 = 512)
        [23] = { .ptr_ptr_size = {memmove_memory_block_1, memmove_memory_block_1 + 256, 256} }, // Perfect half-buffer seam left
        [24] = { .ptr_ptr_size = {memmove_memory_block_1, memmove_memory_block_2,       512} }, // Copy exactly the entire 512-byte limit
        [25] = { .ptr_ptr_size = {memmove_memory_block_1 + 511, memmove_memory_block_2,   1} }, // Copy to the exact last byte of the 512 buffer (Index 511)
    },
    .test_count               = 26,
    .name                     = String8Literal("ft_memmove"),
    .file                     = String8Literal(__FILE__),
    .function_return_type     = TestReturnType_Ptr,
    .function_parameters_type = TestParametersType_PtrPtrSize,
    .libft_function           = (void *)ft_memmove,
    .callback                 = (TestCallbackFunction)callback_for_memmove,
};
