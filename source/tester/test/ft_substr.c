internal_function
TestPayload callback_for_substr(TestParameters test_parameters)
{
    TestPayload payload = {0};

    TestParameters_PtrSizeSizePtr parameters = test_parameters.ptr_size_size_ptr;

    char *expected_return = (char *)parameters.ptr2;
    U64 expected_return_length;
    if(expected_return != 0)
    {
        expected_return_length = length_of_cstring(expected_return) + 1;
        MemoryCopy(payload.buffer_for_expected, expected_return, expected_return_length);
        payload.expected_strings_sizes[0]    = expected_return_length;
        payload.expected_strings_sizes_count = 1;
    }

    // Reset thread_static_allocation_count before calling libft function.
    thread_static_allocation_count = 0;

    // Call libft function and save the result.
    char *got_return = ft_substr((const char *)parameters.ptr1, (unsigned int)parameters.size1, (size_t)parameters.size2);
    U64 got_return_length;
    if(got_return != 0)
    {
        got_return_length = length_of_cstring(got_return) + 1;
        MemoryCopy(payload.buffer_for_got, got_return, got_return_length);
        payload.got_strings_sizes[0]    = got_return_length;
        payload.got_strings_sizes_count = 1;

        if((expected_return != 0) &&
           (expected_return_length == got_return_length) &&
           (MemoryMatch(payload.buffer_for_got, payload.buffer_for_expected, expected_return_length)))
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

    return(payload);
}

read_only global TestGroup test_group_ft_substr =
{
    .tests =
    {
        [0]  = { .ptr_size_size_ptr = {"Hello World", 0, 5, "Hello"} },
        [1]  = { .ptr_size_size_ptr = {"Hello World", 6, 5, "World"} },
        [2]  = { .ptr_size_size_ptr = {"Hello World", 2, 4, "llo "} },
        [3]  = { .ptr_size_size_ptr = {"Hello World", 6, 100, "World"} },
        [4]  = { .ptr_size_size_ptr = {"Hello",       0, 42,  "Hello"} },
        [5]  = { .ptr_size_size_ptr = {"Hello",       5,  2, ""} }, // Exactly at '\0'
        [6]  = { .ptr_size_size_ptr = {"Hello",       10, 5, ""} }, // Way past '\0'
        [7]  = { .ptr_size_size_ptr = {"Hello", (unsigned int)-1, 5, ""} }, // UINT_MAX start
        [8]  = { .ptr_size_size_ptr = {"Hello",       2,  0, ""} },
        [9]  = { .ptr_size_size_ptr = {"Hello",       10, 0, ""} },
        [10] = { .ptr_size_size_ptr = {"",            0,  0, ""} },
        [11] = { .ptr_size_size_ptr = {"",            1,  1, ""} },
        [12] = { .ptr_size_size_ptr = {"42 Prague",   0,  (size_t)-1, "42 Prague"} },
        [13] = { .ptr_size_size_ptr = {"42 Prague",   3,  (size_t)-1, "Prague"} },
        [14] = { .ptr_size_size_ptr = {"42 Prague",   99, (size_t)-1, ""} }, // Huge len, but start is out of bounds
    },
    .test_count               = 15,
    .name                     = String8Literal("ft_substr"),
    .file                     = String8Literal(__FILE__),
    .function_return_type     = TestReturnType_Ptr,
    .function_parameters_type = TestParametersType_PtrSizeSize,
    .libft_function           = (void *)ft_substr,
    .callback                 = (TestCallbackFunction)callback_for_substr,
};
