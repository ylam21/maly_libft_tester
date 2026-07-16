internal_function
TestPayload callback_for_tolower(TestParameters test_parameters)
{
    TestPayload payload = {0};

    TestParameters_Int parameters = test_parameters.single_int;

    // Call libc function.
    int expected_return = tolower((int)parameters.a);

    // Reset thread_static_allocation_count before calling libft function.
    thread_static_allocation_count = 0;

    // Call libft function.
    int got_return = ft_tolower((int)parameters.a);

    if(expected_return == got_return)
    {
        payload.flags |= TestPayloadFlag_ResultsMatch;
    }

    payload.leak_count     = thread_static_allocation_count;
    payload.expected_value = (U64)expected_return;
    payload.got_value      = (U64)got_return;

    return(payload);
}

read_only global TestGroup test_group_ft_tolower =
{
    .tests =
    {
        // Note: tolower argument 'c' must have the value of an unsigned char or EOF. (https://man7.org/linux/man-pages/man3/tolower.3p.html)
        //       Do not write any test that does not follow this rule.
        // Note: Unsigned char has the range 0 - 255.
        [0]  = { .single_int = {EOF} },
        [1]  = { .single_int = {0  } },
        [2]  = { .single_int = {255} }, // Max unsigned char
        [3]  = { .single_int = {'A'    } }, // 65 -> 97 ('a')
        [4]  = { .single_int = {'A' - 1} }, // 64 '@' -> Must remain unchanged
        [5]  = { .single_int = {'Z'    } }, // 90 -> 122 ('z')
        [6]  = { .single_int = {'Z' + 1} }, // 91 '[' -> Must remain unchanged
        [7]  = { .single_int = {'M'    } },
        [8]  = { .single_int = {'a'    } },
        [9]  = { .single_int = {'a' - 1} }, // 96 '`' -> Must remain unchanged
        [10] = { .single_int = {'z'    } },
        [11] = { .single_int = {'z' + 1} }, // 123 '{' -> Must remain unchanged
        [12] = { .single_int = {'0' } },
        [13] = { .single_int = {'\n'} },
        [14] = { .single_int = {' ' } },
    },
    .test_count               = 15,
    .name                     = String8Literal("ft_tolower"),
    .file                     = String8Literal(__FILE__),
    .function_return_type     = TestReturnType_Int,
    .function_parameters_type = TestParametersType_Int,
    .libft_function           = (void *)ft_tolower,
    .callback                 = (TestCallbackFunction)callback_for_tolower,
};
