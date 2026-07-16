internal_function
TestPayload callback_for_strtrim(TestParameters test_parameters)
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
    char *got_return = ft_strtrim((const char *)parameters.ptr1, (const char *)parameters.ptr2);
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

read_only global TestGroup test_group_ft_strtrim =
{
    .tests =
    {
        [0]  = { .ptr_ptr_ptr = {"   Trim Me     ",          " ",         "Trim Me"} },
        [1]  = { .ptr_ptr_ptr = {"xxxHello Worldyyy",        "xy",        "Hello World"} },
        [2]  = { .ptr_ptr_ptr = {"ababaHello Worldbabab",    "ab",        "Hello World"} },
        [3]  = { .ptr_ptr_ptr = {"     Left Trim",           " ",         "Left Trim"} },
        [4]  = { .ptr_ptr_ptr = {"Right Trim     ",          " ",         "Right Trim"} },
        [5]  = { .ptr_ptr_ptr = {"lorem ipsum dolor sit amet", "l",       "orem ipsum dolor sit amet"} },
        [6]  = { .ptr_ptr_ptr = {"lorem ipsum dolor sit amet", "t",       "lorem ipsum dolor sit ame"} },
        [7]  = { .ptr_ptr_ptr = {"  Hello  World  ",         " ",         "Hello  World"} },
        [8]  = { .ptr_ptr_ptr = {"xxxHelloxxWorldxxx",       "x",         "HelloxxWorld"} },
        [9]  = { .ptr_ptr_ptr = {"          ",               " ",         ""} },
        [10] = { .ptr_ptr_ptr = {"ababababab",               "ab",        ""} },
        [11] = { .ptr_ptr_ptr = {"Hello World",              "xyz",       "Hello World"} },
        [12] = { .ptr_ptr_ptr = {"",                         " ",         ""} },
        [13] = { .ptr_ptr_ptr = {"Hello World",              "",          "Hello World"} },
        [14] = { .ptr_ptr_ptr = {"",                         "",          ""} },
        [15] = { .ptr_ptr_ptr = {" \t\n\v\f\rHello World \t\n\v\f\r", " \t\n\v\f\r", "Hello World"} },
    },
    .test_count               = 16,
    .name                     = String8Literal("ft_strtrim"),
    .file                     = String8Literal(__FILE__),
    .function_return_type     = TestReturnType_Ptr,
    .function_parameters_type = TestParametersType_PtrPtr,
    .libft_function           = (void *)ft_strtrim,
    .callback                 = (TestCallbackFunction)callback_for_strtrim,
};
