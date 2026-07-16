global U8 strlcat_memory_block[TESTER_MAXIMUM_PAYLOAD_BUFFER_SIZE];

internal_function
TestPayload callback_for_strlcat(TestParameters test_parameters)
{
    TestPayload payload = {0};

    TestParameters_PtrPtrSize parameters = test_parameters.ptr_ptr_size;

    memory_set_u32(strlcat_memory_block, 0xDEADBEEF, TESTER_MAXIMUM_PAYLOAD_BUFFER_SIZE);
    String8 prefix_string = String8Literal("Hello, I am the prefix!>");
    MemoryCopy(strlcat_memory_block, prefix_string.str, prefix_string.size + 1);

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

    // Reset thread_static_allocation_count before calling libft function.
    thread_static_allocation_count = 0;

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

    payload.leak_count     = thread_static_allocation_count;
    payload.expected_value = (U64)expected_return;
    payload.got_value      = (U64)got_return;

    return(payload);
}


read_only global TestGroup test_group_ft_strlcat =
{
    .tests =
    {
        [0]  = { .ptr_ptr_size = {strlcat_memory_block, "World",       50} },
        [1]  = { .ptr_ptr_size = {strlcat_memory_block, "42",          50} },
        [2]  = { .ptr_ptr_size = {strlcat_memory_block, "World",       30} }, // 24 + 5 + 1 = 30
        [3]  = { .ptr_ptr_size = {strlcat_memory_block, "World",       28} }, // Appends "Wor\0"
        [4]  = { .ptr_ptr_size = {strlcat_memory_block, "A",           25} }, // Appends "\0" only
        [5]  = { .ptr_ptr_size = {strlcat_memory_block, "World",       24} },
        [6]  = { .ptr_ptr_size = {strlcat_memory_block, "World",       15} },
        [7]  = { .ptr_ptr_size = {strlcat_memory_block, "World",       2} },
        [8]  = { .ptr_ptr_size = {strlcat_memory_block, "Do not copy", 0} },
        [9]  = { .ptr_ptr_size = {strlcat_memory_block, "",            0} },
        [10] = { .ptr_ptr_size = {strlcat_memory_block, "",            50} }, // Plenty of space
        [11] = { .ptr_ptr_size = {strlcat_memory_block, "",            10} }, // dstsize < strlen(dst)
        [12] = { .ptr_ptr_size = {strlcat_memory_block, "Short",       TESTER_MAXIMUM_PAYLOAD_BUFFER_SIZE} },
    },
    .test_count               = 13,
    .name                     = String8Literal("ft_strlcat"),
    .file                     = String8Literal(__FILE__),
    .function_return_type     = TestReturnType_Size,
    .function_parameters_type = TestParametersType_PtrPtrSize,
    .libft_function           = (void *)ft_strlcat,
    .callback                 = (TestCallbackFunction)callback_for_strlcat,
};
