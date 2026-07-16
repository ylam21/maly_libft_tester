internal_function
TestPayload callback_for_strjoin(TestParameters test_parameters)
{
    TestPayload payload = {0};

    TestParameters_PtrPtrPtr parameters = test_parameters.ptr_ptr_ptr;

    char *expected_return = parameters.ptr3; // Note: Null-terminated string is expected.
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
    char *got_return = ft_strjoin((const char *)parameters.ptr1, (const char *)parameters.ptr2);
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

read_only global TestGroup test_group_ft_strjoin =
{
    .tests =
    {
        [0]  = { .ptr_ptr_ptr = {"Hello ", "World!",    "Hello World!"} },
        [1]  = { .ptr_ptr_ptr = {"42",     "Prague",    "42Prague"} },
        [2]  = { .ptr_ptr_ptr = {"Join",   "Me",        "JoinMe"} },
        [3]  = { .ptr_ptr_ptr = {"",       "",          ""} },
        [4]  = { .ptr_ptr_ptr = {"Left",   "",          "Left"} },
        [5]  = { .ptr_ptr_ptr = {"",       "Right",     "Right"} },
        [6]  = { .ptr_ptr_ptr = {"A",      "B",         "AB"} },
        [7]  = { .ptr_ptr_ptr = {" ",      " ",         "  "} },
        [8]  = { .ptr_ptr_ptr = {"\n\t",   " \v\r",     "\n\t \v\r"} },
        [9]  = { .ptr_ptr_ptr = {"   ",    "   ",       "      "} },
        [10] = { .ptr_ptr_ptr = {"123",    "456",       "123456"} },
        [11] = { .ptr_ptr_ptr = {"!@#",    "$%^",       "!@#$%^"} },
        [12] = { .ptr_ptr_ptr = {"This is a very long string designed to test ", "if they allocate enough memory.", "This is a very long string designed to test if they allocate enough memory."} },
        [13] = { .ptr_ptr_ptr = {"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA", "BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB", "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAABBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB"} },
    },
    .test_count               = 14,
    .name                     = String8Literal("ft_strjoin"),
    .file                     = String8Literal(__FILE__),
    .function_return_type     = TestReturnType_Ptr,
    .function_parameters_type = TestParametersType_PtrPtr,
    .libft_function           = (void *)ft_strjoin,
    .callback                 = (TestCallbackFunction)callback_for_strjoin,
};
