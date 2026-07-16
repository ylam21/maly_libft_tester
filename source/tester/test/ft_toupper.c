internal_function
TestPayload callback_for_toupper(TestParameters test_parameters)
{
    TestPayload payload = {0};

    TestParameters_Int parameters = test_parameters.single_int;

    // Call libc function.
    int expected_return = toupper((int)parameters.a);

    // Reset thread_static_allocation_count before calling libft function.
    thread_static_allocation_count = 0;

    // Call libft function.
    int got_return = ft_toupper((int)parameters.a);

    if(expected_return == got_return)
    {
        payload.flags |= TestPayloadFlag_ResultsMatch;
    }

    payload.leak_count     = thread_static_allocation_count;
    payload.expected_value = (U64)expected_return;
    payload.got_value      = (U64)got_return;

    return(payload);
}

read_only global TestGroup test_group_ft_toupper =
{
    .tests =
    {
        // Note: toupper argument 'c' must have the value of an unsigned char or EOF. (https://linux.die.net/man/3/toupper)
        //       Do not write any test that does not follow this rule.
        // Note: Unsigned char has the range 0 - 255.
        [0]  = { .single_int = {EOF} },
        [1]  = { .single_int = {0  } },
        [2]  = { .single_int = {255} }, // Max unsigned char
        [3]  = { .single_int = {'a'    } }, // 97 -> 65 ('A')
        [4]  = { .single_int = {'a' - 1} }, // 96 '`' -> Must remain unchanged
        [5]  = { .single_int = {'z'    } }, // 122 -> 90 ('Z')
        [6]  = { .single_int = {'z' + 1} }, // 123 '{' -> Must remain unchanged
        [7]  = { .single_int = {'m'    } },
        [8]  = { .single_int = {'A'    } },
        [9]  = { .single_int = {'A' - 1} }, // 64 '@' -> Must remain unchanged
        [10] = { .single_int = {'Z'    } },
        [11] = { .single_int = {'Z' + 1} }, // 91 '[' -> Must remain unchanged
        [12] = { .single_int = {'0' } },
        [13] = { .single_int = {'\n'} },
        [14] = { .single_int = {' ' } },
    },
    .test_count               = 15,
    .name                     = String8Literal("ft_toupper"),
    .file                     = String8Literal(__FILE__),
    .function_return_type     = TestReturnType_Int,
    .function_parameters_type = TestParametersType_Int,
    .libft_function           = (void *)ft_toupper,
    .callback                 = (TestCallbackFunction)callback_for_toupper,
};
