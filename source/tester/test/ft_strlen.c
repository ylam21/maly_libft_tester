internal_function
TestPayload callback_for_strlen(TestParameters test_parameters)
{
    TestPayload payload = {0};

    TestParameters_Ptr parameters = test_parameters.ptr;

    // Call libc function.
    size_t expected_return = strlen((const char *)parameters.ptr);

    // Reset thread_static_allocation_count before calling libft function.
    thread_static_allocation_count = 0;

    // Call libft function.
    size_t got_return = ft_strlen((const char *)parameters.ptr);

    if(expected_return == got_return)
    {
        payload.flags |= TestPayloadFlag_ResultsMatch;
    }

    payload.leak_count     = thread_static_allocation_count;
    payload.expected_value = (U64)expected_return;
    payload.got_value      = (U64)got_return;

    return(payload);
}

read_only global TestGroup test_group_ft_strlen =
{
    .tests =
    {
        [0]  = { .ptr = {"" } },
        [1]  = { .ptr = {"a"} },
        [2]  = { .ptr = {" "} },
        [3]  = { .ptr = {"0"} },
        [4]  = { .ptr = {"Hello\0World"} },
        [5]  = { .ptr = {"\0Hidden"    } },
        [6]  = { .ptr = {"a\0b\0c"     } },
        [7]  = { .ptr = {"Hello, 42!"  } },
        [8]  = { .ptr = {"123456"      } },
        [9]  = { .ptr = {"\a\b\t\n\v\f\r"} }, // True control characters
        [10] = { .ptr = {"\\0"           } }, // Literal backslash-zero (len 2)
        [11] = { .ptr = {"/0"            } }, // Literal forward slash-zero (len 2)
        [12] = { .ptr = {"©®™"         } },
        [13] = { .ptr = {"\xFF\xAA\x01"} },   // Arbitrary non-ascii hex bytes
        [14] = { .ptr = {"A very long string... A very long string...  A very long string...  A very long string...  A very long string...  A very long string...  A very long string...  A very long string...  A very long string...  A very long string...  A very long string...  A very long string...  A very long string...  A very long string...  A very long string...  A very long string...   "} },
    },
    .test_count               = 15,
    .name                     = String8Literal("ft_strlen"),
    .file                     = String8Literal(__FILE__),
    .function_return_type     = TestReturnType_Size,
    .function_parameters_type = TestParametersType_Ptr,
    .libft_function           = (void *)ft_strlen,
    .callback                 = (TestCallbackFunction)callback_for_strlen,
};
