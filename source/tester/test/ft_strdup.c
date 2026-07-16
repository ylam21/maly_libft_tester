internal_function
TestPayload callback_for_strdup(TestParameters test_parameters)
{
    TestPayload payload = {0};

    TestParameters_Ptr parameters = test_parameters.ptr;

    // Call libc function and save the result.
    char *expected_return = strdup((const char *)parameters.ptr); // Note: Null-terminated string is expected.
    U64 expected_return_length;
    if(expected_return != 0)
    {
        expected_return_length = length_of_cstring(expected_return) + 1;
        MemoryCopy(payload.buffer_for_expected, expected_return, expected_return_length);
        payload.expected_strings_sizes[0]    = expected_return_length;
        payload.expected_strings_sizes_count = 1;

        free(expected_return);
    }

    // Reset thread_static_allocation_count before calling libft function.
    thread_static_allocation_count = 0;

    // Call libft function and save the result.
    char *got_return = ft_strdup((const char *)parameters.ptr);
    if(got_return != 0)
    {
        U64 got_return_length = length_of_cstring(got_return) + 1;
        MemoryCopy(payload.buffer_for_got, got_return, got_return_length);
        payload.got_strings_sizes[0]    = got_return_length;
        payload.got_strings_sizes_count = 1;

        if((expected_return != 0) && (expected_return_length == got_return_length) && MemoryMatch(payload.buffer_for_got, payload.buffer_for_expected, expected_return_length))
        {
            payload.flags |= TestPayloadFlag_ResultsMatch;
        }

        free(got_return);
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

read_only global TestGroup test_group_ft_strdup =
{
    .tests =
    {
        [0]  = { .ptr = {""} },
        [1]  = { .ptr = {"a"} },
        [2]  = { .ptr = {"\n"} },
        [3]  = { .ptr = {"Hello, World!"} },
        [4]  = { .ptr = {"42 Prague"} },
        [5]  = { .ptr = {"0123456789"} },
        [6]  = { .ptr = {"Hidden\0Secret"} },
        [7]  = { .ptr = {"\0StartNull"} },
        [8]  = { .ptr = {" \t\n\v\f\r "} },
        [9]  = { .ptr = {"\\\\Double Backslash\\\\"} },
        [10] = { .ptr = {"\033[31mRed Alert\033[0m"} },
        [11] = { .ptr = {"©®™"} },
        [12] = { .ptr = {"\xff\xaa\x01\x02"} },
        [13] = { .ptr = {"This is a significantly longer string designed to test if the student's malloc logic correctly handles sizes beyond small, single-word buffers. It needs to be long enough to expose potential off-by-one errors when iterating."} }
    },
    .test_count               = 14,
    .name                     = String8Literal("ft_strdup"),
    .file                     = String8Literal(__FILE__),
    .function_return_type     = TestReturnType_Ptr,
    .function_parameters_type = TestParametersType_Ptr,
    .libft_function           = (void *)ft_strdup,
    .callback                 = (TestCallbackFunction)callback_for_strdup,
};
