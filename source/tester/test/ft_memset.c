global U8 memset_memory_block[TESTER_MAXIMUM_PAYLOAD_BUFFER_SIZE];

internal_function
TestPayload callback_for_memset(TestParameters test_parameters)
{
    TestPayload payload = {0};

    TestParameters_PtrIntSize parameters = test_parameters.ptr_int_size;

    memory_set_u32(memset_memory_block, 0xDEADBEEF, TESTER_MAXIMUM_PAYLOAD_BUFFER_SIZE);

    // Backup memory to backup buffer.
    U8 backup_buffer[TESTER_MAXIMUM_PAYLOAD_BUFFER_SIZE];
    MemoryCopy(backup_buffer, parameters.ptr, parameters.size);

    // Call libc function and save the result.
    void *expected_return = memset((void *)parameters.ptr, (int)parameters.a, (size_t)parameters.size);
    if(expected_return != 0)
    {
        MemoryCopy(payload.buffer_for_expected, expected_return, parameters.size);
        payload.expected_strings_sizes[0]    = parameters.size;
        payload.expected_strings_sizes_count = 1;
    }

    // Restore memory with backup buffer.
    MemoryCopy(parameters.ptr, backup_buffer, parameters.size);

    // Reset thread_static_allocation_count before calling libft function.
    thread_static_allocation_count = 0;

    // Call libft function and save the result.
    void *got_return = ft_memset((void *)parameters.ptr, (int)parameters.a, (size_t)parameters.size);
    if(got_return != 0)
    {
        MemoryCopy(payload.buffer_for_got, got_return, parameters.size);
        payload.got_strings_sizes[0]    = parameters.size;
        payload.got_strings_sizes_count = 1;
    }

    if((got_return == expected_return) && MemoryMatch(payload.buffer_for_expected, payload.buffer_for_got, parameters.size))
    {
        payload.flags |= TestPayloadFlag_ResultsMatch;
    }

    payload.leak_count     = thread_static_allocation_count;
    payload.expected_value = (U64)expected_return;
    payload.got_value      = (U64)got_return;

    return(payload);
}


read_only global TestGroup test_group_ft_memset =
{
    .tests =
    {
        [0]  = { .ptr_int_size = {memset_memory_block, 'A', 10               } },
        [1]  = { .ptr_int_size = {memset_memory_block, 0,   32               } },
        [2]  = { .ptr_int_size = {memset_memory_block, 'X', TESTER_MAXIMUM_PAYLOAD_BUFFER_SIZE} },
        [3]  = { .ptr_int_size = {memset_memory_block, 'A', 0                } },
        [4]  = { .ptr_int_size = {memset_memory_block + 5, 'B', 0            } }, // Unaligned 0 size
        [5]  = { .ptr_int_size = {memset_memory_block, 256, 10               } }, // 256 % 256 = 0
        [6]  = { .ptr_int_size = {memset_memory_block, -1,  10               } }, // -1 becomes 255
        [7]  = { .ptr_int_size = {memset_memory_block, -42, 15               } },
        [8]  = { .ptr_int_size = {memset_memory_block, 2147483647, 5         } }, // INT_MAX
        [9]  = { .ptr_int_size = {memset_memory_block, 'Z', 1                } },
        [10] = { .ptr_int_size = {memset_memory_block, 'Y', 17               } }, // Prime number size
        [11] = { .ptr_int_size = {memset_memory_block + 1, 'C', 42           } },
        [12] = { .ptr_int_size = {memset_memory_block + 3, 'D', 15           } },
        [13] = { .ptr_int_size = {memset_memory_block + 7, 'E', 1            } },
        [14] = { .ptr_int_size = {memset_memory_block + 1, 'F',   TESTER_MAXIMUM_PAYLOAD_BUFFER_SIZE - 1} },
        [15] = { .ptr_int_size = {memset_memory_block + 127, 'G', TESTER_MAXIMUM_PAYLOAD_BUFFER_SIZE - 127} },
    },
    .test_count               = 16,
    .name                     = String8Literal("ft_memset"),
    .file                     = String8Literal(__FILE__),
    .function_return_type     = TestReturnType_Ptr,
    .function_parameters_type = TestParametersType_PtrIntSize,
    .libft_function           = (void *)ft_memset,
    .callback                 = (TestCallbackFunction)callback_for_memset,
};
