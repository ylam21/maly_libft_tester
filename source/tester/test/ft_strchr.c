internal_function
TestPayload callback_for_strchr(TestParameters test_parameters)
{
    TestPayload payload = {0};

    TestParameters_PtrInt parameters = test_parameters.ptr_int;

    // Call libc function and save the result.
    char *expected_return = strchr((const char *)parameters.ptr, (int)parameters.a);
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
    char *got_return = ft_strchr((const char *)parameters.ptr, (int)parameters.a);
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

read_only global TestGroup test_group_ft_strchr =
{
    .tests =
    {
        [0]  = { .ptr_int = {"Hello World", 'H'} },
        [1]  = { .ptr_int = {"Hello World", 'W'} },
        [2]  = { .ptr_int = {"Hello World", 'd'} },
        [3]  = { .ptr_int = {"lorem ipsum dolor", 'o'} },
        [4]  = { .ptr_int = {"abababab", 'b'} },
        [5]  = { .ptr_int = {"Hello World", 'z'} },
        [6]  = { .ptr_int = {"42 Prague", 'x'} },
        [7]  = { .ptr_int = {"Hello World", '\0'} },
        [8]  = { .ptr_int = {"", '\0'} }, // Should return pointer to the '\0'
        [9]  = { .ptr_int = {"", 'a'} },  // Should return NULL
        [10] = { .ptr_int = {"Hello", 'H' + 256} },
        [11] = { .ptr_int = {"Hello", 'e' - 256} },
        [12] = { .ptr_int = {"lorem\xffipsum", '\xff'} },
    },
    .test_count               = 13,
    .name                     = String8Literal("ft_strchr"),
    .file                     = String8Literal(__FILE__),
    .function_return_type     = TestReturnType_Ptr,
    .function_parameters_type = TestParametersType_PtrInt,
    .libft_function           = (void *)ft_strchr,
    .callback                 = (TestCallbackFunction)callback_for_strchr,
};
