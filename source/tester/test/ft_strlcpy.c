global U8 strlcpy_memory_block[TESTER_MAXIMUM_PAYLOAD_BUFFER_SIZE];

internal_function
TestPayload callback_for_strlcpy(TestParameters test_parameters)
{
    TestPayload payload = {0};

    TestParameters_PtrPtrSize parameters = test_parameters.ptr_ptr_size;

    memory_set_u32(strlcpy_memory_block, 0xDEADBEEF, TESTER_MAXIMUM_PAYLOAD_BUFFER_SIZE);

    // Backup memory to backup buffer.
    U8 backup_buffer[TESTER_MAXIMUM_PAYLOAD_BUFFER_SIZE];
    MemoryCopy(backup_buffer, parameters.ptr1, TESTER_MAXIMUM_PAYLOAD_BUFFER_SIZE);

    // Call libc function and save the result.
    size_t expected_return = strlcpy((void *)parameters.ptr1, (const void *)parameters.ptr2, (size_t)parameters.size);

    U64 expected_return_length = expected_return + 1;
    MemoryCopy(payload.buffer_for_expected, parameters.ptr1, expected_return_length);
    payload.expected_strings_sizes[0]    = expected_return_length;
    payload.expected_strings_sizes_count = 1;

    // Restore memory with backup buffer.
    MemoryCopy(parameters.ptr1, backup_buffer, TESTER_MAXIMUM_PAYLOAD_BUFFER_SIZE);

    // Reset thread_static_allocation_count before calling libft function.
    thread_static_allocation_count = 0;

    // Call libft function and save the result.
    size_t got_return = ft_strlcpy((void *)parameters.ptr1, (const void *)parameters.ptr2, (size_t)parameters.size);

    U64 got_return_length = got_return + 1;
    MemoryCopy(payload.buffer_for_got, parameters.ptr1, got_return_length);
    payload.got_strings_sizes[0]    = got_return_length;
    payload.got_strings_sizes_count = 1;

    if((got_return_length == expected_return_length) && (MemoryMatch(payload.buffer_for_got, payload.buffer_for_expected, expected_return_length)))
    {
        payload.flags |= TestPayloadFlag_ResultsMatch;
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


read_only global TestGroup test_group_ft_strlcpy =
{
    .tests =
    {
        [0] = { .ptr_ptr_size = {strlcpy_memory_block, "Hello",        10} },
        [1] = { .ptr_ptr_size = {strlcpy_memory_block, "42",            5} },
        [2] = { .ptr_ptr_size = {strlcpy_memory_block, "Hello World",  12} },
        [3] = { .ptr_ptr_size = {strlcpy_memory_block, "Hello World",   5} },
        [4] = { .ptr_ptr_size = {strlcpy_memory_block, "42 Prague",     2} },
        [5] = { .ptr_ptr_size = {strlcpy_memory_block, "A",             1} }, // 1 byte = only '\0'
        [6] = { .ptr_ptr_size = {strlcpy_memory_block, "Do not copy",   0} },
        [7] = { .ptr_ptr_size = {strlcpy_memory_block, "",              0} },
        [8] = { .ptr_ptr_size = {strlcpy_memory_block, "",             10} },
        [9] = { .ptr_ptr_size = {strlcpy_memory_block, "Short",       100} },
    },
    .test_count               = 10,
    .name                     = String8Literal("ft_strlcpy"),
    .file                     = String8Literal(__FILE__),
    .function_return_type     = TestReturnType_Size,
    .function_parameters_type = TestParametersType_PtrPtrSize,
    .libft_function           = (void *)ft_strlcpy,
    .callback                 = (TestCallbackFunction)callback_for_strlcpy,
};
