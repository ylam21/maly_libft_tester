global U8 memcpy_memory_block_1[TESTER_MAXIMUM_PAYLOAD_BUFFER_SIZE];
global U8 memcpy_memory_block_2[TESTER_MAXIMUM_PAYLOAD_BUFFER_SIZE];

internal_function
TestPayload callback_for_memcpy(TestParameters test_parameters)
{
    TestPayload payload = {0};

    TestParameters_PtrPtrSize parameters = test_parameters.ptr_ptr_size;

    memory_set_u32(memcpy_memory_block_1, 0xDEADBEEF, TESTER_MAXIMUM_PAYLOAD_BUFFER_SIZE);
    memory_set_u32(memcpy_memory_block_2, 0x12345678, TESTER_MAXIMUM_PAYLOAD_BUFFER_SIZE);

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

    // Reset thread_static_allocation_count before calling libft function.
    thread_static_allocation_count = 0;

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

    payload.leak_count     = thread_static_allocation_count;
    payload.expected_value = (U64)expected_return;
    payload.got_value      = (U64)got_return;

    return(payload);
}


read_only global TestGroup test_group_ft_memcpy =
{

    .tests =
    {
        // Note: "...The memory areas must not overlap. Use memmove(3) if the memory areas do overlap." (https://man7.org/linux/man-pages/man3/memcpy.3.html)
        //        Do not write tests where memory areas overlap.
        [0]  = { .ptr_ptr_size = {memcpy_memory_block_1,      memcpy_memory_block_2,  10} },
        [1]  = { .ptr_ptr_size = {memcpy_memory_block_1,      memcpy_memory_block_2, 128} },
        [2]  = { .ptr_ptr_size = {memcpy_memory_block_1,      memcpy_memory_block_2, TESTER_MAXIMUM_PAYLOAD_BUFFER_SIZE} }, // Full buffer
        [3]  = { .ptr_ptr_size = {memcpy_memory_block_1,      memcpy_memory_block_2,     0} },
        [4]  = { .ptr_ptr_size = {memcpy_memory_block_1 + 50, memcpy_memory_block_2 + 50,0} },
        [5]  = { .ptr_ptr_size = {0, 0, 0} }, // Standard protection test
        [6]  = { .ptr_ptr_size = {memcpy_memory_block_1 + 1, memcpy_memory_block_2,       42} },
        [7]  = { .ptr_ptr_size = {memcpy_memory_block_1 + 3, memcpy_memory_block_2,       15} },
        [8]  = { .ptr_ptr_size = {memcpy_memory_block_1 + 7, memcpy_memory_block_2,      113} },
        [9]  = { .ptr_ptr_size = {memcpy_memory_block_1,     memcpy_memory_block_2 + 1,   42} },
        [10] = { .ptr_ptr_size = {memcpy_memory_block_1,     memcpy_memory_block_2 + 3,   15} },
        [11] = { .ptr_ptr_size = {memcpy_memory_block_1,     memcpy_memory_block_2 + 7,  113} },
        [12] = { .ptr_ptr_size = {memcpy_memory_block_1 + 1, memcpy_memory_block_2 + 3,  117} },
        [13] = { .ptr_ptr_size = {memcpy_memory_block_1 + 3, memcpy_memory_block_2 + 1,  117} },
        [14] = { .ptr_ptr_size = {memcpy_memory_block_1 + 7, memcpy_memory_block_2 + 2,   89} },
    },
    .test_count               = 15,
    .name                     = String8Literal("ft_memcpy"),
    .file                     = String8Literal(__FILE__),
    .function_return_type     = TestReturnType_Ptr,
    .function_parameters_type = TestParametersType_PtrPtrSize,
    .libft_function           = (void *)ft_memcpy,
    .callback                 = (TestCallbackFunction)callback_for_memcpy,
};
