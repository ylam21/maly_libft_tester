global U8 bzero_memory_block[TESTER_MAXIMUM_PAYLOAD_BUFFER_SIZE];

internal_function
TestPayload callback_for_bzero(TestParameters test_parameters)
{
    TestPayload payload = {0};

    TestParameters_PtrSize parameters = test_parameters.ptr_size;

    memory_set_u32(bzero_memory_block, 0xDEADBEEF, TESTER_MAXIMUM_PAYLOAD_BUFFER_SIZE);

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

    // Reset thread_static_allocation_count before calling libft function.
    thread_static_allocation_count = 0;

    // Call libft function and save the result.
    ft_bzero((void *)parameters.ptr, (size_t)parameters.size);
    MemoryCopy(payload.buffer_for_got, parameters.ptr, parameters.size);
    payload.got_strings_sizes[0]    = parameters.size;
    payload.got_strings_sizes_count = 1;

    if(MemoryMatch(payload.buffer_for_expected, payload.buffer_for_got, parameters.size))
    {
        payload.flags |= TestPayloadFlag_ResultsMatch;
    }

    payload.leak_count = thread_static_allocation_count;

    return(payload);
}


read_only global TestGroup test_group_ft_bzero =
{
    .tests =
    {
        // Note: Passing NULL pointer to bzero results in undefined behaviour. Do not write such tests.
        // Note: Make sure we do not touch any memory out of bounds this memory block.
        [0]  = { .ptr_size = {bzero_memory_block,  10} },
        [1]  = { .ptr_size = {bzero_memory_block,  42} },
        [2]  = { .ptr_size = {bzero_memory_block, 100} },
        [3]  = { .ptr_size = {bzero_memory_block,      0} },
        [4]  = { .ptr_size = {bzero_memory_block + 50, 0} },
        [5]  = { .ptr_size = {bzero_memory_block, 1}     },
        [6]  = { .ptr_size = {bzero_memory_block + 7, 1} },
        [7]  = { .ptr_size = {bzero_memory_block + 1, 42} },
        [8]  = { .ptr_size = {bzero_memory_block + 3, 42} },
        [9]  = { .ptr_size = {bzero_memory_block + 7, 42} },
        [10] = { .ptr_size = {bzero_memory_block,  7} },
        [11] = { .ptr_size = {bzero_memory_block, 15} },
        [12] = { .ptr_size = {bzero_memory_block, 33} },
        [13] = { .ptr_size = {bzero_memory_block + 5,  37} },
        [14] = { .ptr_size = {bzero_memory_block + 11, 89} },
        [15] = { .ptr_size = {bzero_memory_block,       TESTER_MAXIMUM_PAYLOAD_BUFFER_SIZE      } },
        [16] = { .ptr_size = {bzero_memory_block + 1,   TESTER_MAXIMUM_PAYLOAD_BUFFER_SIZE -   1} },
        [17] = { .ptr_size = {bzero_memory_block + 127, TESTER_MAXIMUM_PAYLOAD_BUFFER_SIZE - 127} },
    },
    .test_count               = 18,
    .name                     = String8Literal("ft_bzero"),
    .file                     = String8Literal(__FILE__),
    .function_return_type     = TestReturnType_Void,
    .function_parameters_type = TestParametersType_PtrSize,
    .libft_function           = (void *)ft_bzero,
    .callback                 = (TestCallbackFunction)callback_for_bzero,
};
