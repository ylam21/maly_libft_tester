internal_function
TestPayload callback_for_strnstr(TestParameters test_parameters)
{
    TestPayload payload = {0};

    TestParameters_PtrPtrSize parameters = test_parameters.ptr_ptr_size;

    // Call libc function and save the result.
    char *expected_return = strnstr((const char *)parameters.ptr1, (const char *)parameters.ptr2, (size_t)parameters.size);
    U64 expected_return_size;
    if(expected_return != 0)
    {
        expected_return_size = length_of_cstring(expected_return) + 1;
        MemoryCopy(payload.buffer_for_expected, expected_return, expected_return_size);
        payload.expected_strings_sizes[0]    = expected_return_size;
        payload.expected_strings_sizes_count = 1;
    }

    // Reset thread_static_allocation_count before calling libft function.
    thread_static_allocation_count = 0;

    // Call libft function and save the result.
    char *got_return = ft_strnstr((const char *)parameters.ptr1, (const char *)parameters.ptr2, (size_t)parameters.size);
    U64 got_return_size;
    if(got_return != 0)
    {
        got_return_size = length_of_cstring(got_return) + 1;
        MemoryCopy(payload.buffer_for_got, got_return, got_return_size);
        payload.got_strings_sizes[0]    = got_return_size;
        payload.got_strings_sizes_count = 1;
    }

    if(got_return == expected_return)
    {
        payload.flags |= TestPayloadFlag_ResultsMatch;
    }

    payload.leak_count     = thread_static_allocation_count;
    payload.expected_value = (U64)expected_return;
    payload.got_value      = (U64)got_return;

    return(payload);
}

read_only global TestGroup test_group_ft_strnstr =
{
    .tests =
    {
        [0]  = { .ptr_ptr_size = {(void *)"Hello World", (void *)"World", 15} },
        [1]  = { .ptr_ptr_size = {(void *)"Hello World", (void *)"Hello", 15} },
        [2]  = { .ptr_ptr_size = {(void *)"Hello World", (void *)"",      15} },
        [3]  = { .ptr_ptr_size = {(void *)"Hello World", (void *)"",      0} },
        [4]  = { .ptr_ptr_size = {(void *)"Hello World", (void *)"World", 11} }, // Fits exactly
        [5]  = { .ptr_ptr_size = {(void *)"Hello World", (void *)"World", 10} }, // Needle goes past len -> NULL
        [6]  = { .ptr_ptr_size = {(void *)"Hello",       (void *)"H",     0} },
        [7]  = { .ptr_ptr_size = {(void *)"aabaabaa",    (void *)"aab",   5} }, // Finds the first one
        [8]  = { .ptr_ptr_size = {(void *)"lorem ipsum", (void *)"ipsum", 9} }, // "ips" is within len, but "um" is not -> NULL
        [9]  = { .ptr_ptr_size = {(void *)"Hello",       (void *)"x",     5} },
        [10] = { .ptr_ptr_size = {(void *)"Hello",       (void *)"HelloX",10} }, // Needle larger than haystack
        [11] = { .ptr_ptr_size = {(void *)"Hi\0There",   (void *)"There", 10} }, // Returns NULL
        [12] = { .ptr_ptr_size = {(void *)"",            (void *)"a",     10} },
        [13] = { .ptr_ptr_size = {(void *)"",            (void *)"",      10} },
    },
    .test_count               = 14,
    .name                     = String8Literal("ft_strnstr"),
    .file                     = String8Literal(__FILE__),
    .function_return_type     = TestReturnType_Ptr,
    .function_parameters_type = TestParametersType_PtrPtrSize,
    .libft_function           = (void *)ft_strnstr,
    .callback                 = (TestCallbackFunction)callback_for_strnstr,
};
