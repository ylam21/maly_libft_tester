internal_function
TestPayload callback_for_calloc(TestParameters test_parameters)
{
    TestPayload payload = {0};

    TestParameters_SizeSize parameters = test_parameters.size_size;

    U64 total_size = parameters.size1 * parameters.size2;
    U64 check_size = MinimumBetween(total_size, TESTER_MAXIMUM_PAYLOAD_BUFFER_SIZE);

    // Call libc function.
    void *expected_return = calloc((size_t)parameters.size1, (size_t)parameters.size2);
    if(expected_return != 0)
    {
        MemoryCopy(payload.buffer_for_expected, expected_return, check_size);
        payload.expected_strings_sizes[0]    = check_size;
        payload.expected_strings_sizes_count = 1;
    }

    // Reset thread_static_allocation_count before calling libft function.
    thread_static_allocation_count = 0;

    // Call libft function.
    void *got_return = ft_calloc((size_t)parameters.size1, (size_t)parameters.size2);
    if(got_return != 0)
    {
        MemoryCopy(payload.buffer_for_got, got_return, check_size);
        payload.got_strings_sizes[0]    = check_size;
        payload.got_strings_sizes_count = 1;

        if((expected_return != 0) && (MemoryMatch(got_return, expected_return, total_size)))
        {
            payload.flags |= TestPayloadFlag_ResultsMatch;
        }

        free(got_return);
    }
    else
    {
        if(expected_return == 0)
        {
            payload.flags |= TestPayloadFlag_ResultsMatch;
        }
    }

    payload.leak_count     = thread_static_allocation_count;
    payload.expected_value = (U64)expected_return;
    payload.got_value      = (U64)got_return;

    free(expected_return);

    return(payload);
}

read_only global TestGroup test_group_ft_calloc =
{
    .tests =
    {
        [0]  = { .size_size = {1,  50} },
        [1]  = { .size_size = {16, 26} },
        [2]  = { .size_size = {10, 10} },
        [3]  = { .size_size = {4,  128} },
        [4]  = { .size_size = {0, 0} },
        [5]  = { .size_size = {8, 0} },
        [6]  = { .size_size = {0, 8} },
        [7]  = { .size_size = {1, 1} },
        [8]  = { .size_size = {1, 0} },
        [9]  = { .size_size = {0, 1} },
        [10] = { .size_size = {100, 10} },
        [11] = { .size_size = {10, 100} },
        [12] = { .size_size = {(size_t)-1, 2} },
        [13] = { .size_size = {2, (size_t)-1} },
        [14] = { .size_size = {(size_t)-1, (size_t)-1} },
    },
    .test_count               = 15,
    .name                     = String8Literal("ft_calloc"),
    .file                     = String8Literal(__FILE__),
    .function_return_type     = TestReturnType_Ptr,
    .function_parameters_type = TestParametersType_SizeSize,
    .libft_function           = (void *)ft_calloc,
    .callback                 = (TestCallbackFunction)callback_for_calloc,
};
