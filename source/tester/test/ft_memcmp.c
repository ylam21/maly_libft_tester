global U8 memcmp_memory_block_1[TESTER_MAXIMUM_PAYLOAD_BUFFER_SIZE];
global U8 memcmp_memory_block_2[TESTER_MAXIMUM_PAYLOAD_BUFFER_SIZE];

internal_function
TestPayload callback_for_memcmp(TestParameters test_parameters)
{
    TestPayload payload = {0};

    TestParameters_PtrPtrSize parameters = test_parameters.ptr_ptr_size;

    memory_set_u32(memcmp_memory_block_1, 0xDEADBEEF, TESTER_MAXIMUM_PAYLOAD_BUFFER_SIZE);
    memory_set_u32(memcmp_memory_block_2, 0xDEADBEEF, TESTER_MAXIMUM_PAYLOAD_BUFFER_SIZE);
    memcmp_memory_block_2[100] = 0x00;

    // Call libc function.
    int expected_return = memcmp((const void *)parameters.ptr1, (const void *)parameters.ptr2, (size_t)parameters.size);

    // Reset thread_static_allocation_count before calling libft function.
    thread_static_allocation_count = 0;

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


read_only global TestGroup test_group_ft_memcmp =
{
    .tests =
    {
        // Note: Standard memcmp evaluates bytes as `unsigned char`.
        [0]  = { .ptr_ptr_size = {(void *)"Hello", (void *)"World", 0} },
        [1]  = { .ptr_ptr_size = {memcmp_memory_block_1, memcmp_memory_block_2,   0} },
        [2]  = { .ptr_ptr_size = {memcmp_memory_block_1, memcmp_memory_block_1, 128} },
        [3]  = { .ptr_ptr_size = {(void *)"Hello World", (void *)"Hello World", 11} },
        [4]  = { .ptr_ptr_size = {(void *)"Hello World", (void *)"Hello Zorld", 11} },
        [5]  = { .ptr_ptr_size = {(void *)"Hello Zorld", (void *)"Hello World", 11} },
        [6]  = { .ptr_ptr_size = {(void *)"\xff\xaa\xbb", (void *)"\x00\xaa\xbb", 3} },
        [7]  = { .ptr_ptr_size = {(void *)"\x00\xaa\xbb", (void *)"\xff\xaa\xbb", 3} },
        [8]  = { .ptr_ptr_size = {(void *)"a\0b\0c", (void *)"a\0b\0c", 5} },
        [9]  = { .ptr_ptr_size = {(void *)"a\0b\0c", (void *)"a\0b\0x", 5} },
        [10] = { .ptr_ptr_size = {(void *)"a\0b\0c", (void *)"a\0b\0x", 3} },
        [11] = { .ptr_ptr_size = {(void *)"42 Prague!!!", (void *)"42 Prague???", 9} }, // Difference is at index 9, size is 9, so it shouldn't be read!
        [12] = { .ptr_ptr_size = {memcmp_memory_block_1, memcmp_memory_block_2, 100} }, // Difference is at 100, so checking 100 bytes should match perfectly (0 to 99)
        [13] = { .ptr_ptr_size = {memcmp_memory_block_1, memcmp_memory_block_2, 101} }, // Checking 101 bytes includes the difference, should NOT match
    },
    .test_count               = 14,
    .name                     = String8Literal("ft_memcmp"),
    .file                     = String8Literal(__FILE__),
    .function_return_type     = TestReturnType_Int,
    .function_parameters_type = TestParametersType_PtrPtrSize,
    .libft_function           = (void *)ft_memcmp,
    .callback                 = (TestCallbackFunction)callback_for_memcmp,
};
