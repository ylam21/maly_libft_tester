internal_function
TestPayload callback_for_isalnum(TestParameters test_parameters)
{
    TestPayload payload = {0};

    TestParameters_Int parameters = test_parameters.single_int;

    // Call libc function.
    int expected_return = isalnum((int)parameters.a);
    // Note from Libft subject (version 19.2):
    //    For the character classification functions (isalpha, isdigit, isalnum, isascii, isprint), the return value must be:
    //    • 1 if the character matches the tested class
    //    • 0 if the character does not match
    expected_return = MinimumBetween(expected_return, 1);

    // Reset thread_static_allocation_count before calling libft function.
    thread_static_allocation_count = 0;

    // Call libft function.
    int got_return = ft_isalnum((int)parameters.a);

    if(expected_return == got_return)
    {
        payload.flags |= TestPayloadFlag_ResultsMatch;
    }

    payload.leak_count     = thread_static_allocation_count;
    payload.expected_value = (U64)expected_return;
    payload.got_value      = (U64)got_return;

    return(payload);
}

read_only global TestGroup test_group_ft_isalnum =
{
    .tests =
    {
        // Note: isalnum argument 'c' must have the value of an unsigned char or EOF. (https://man7.org/linux/man-pages/man3/isspace.3.html)
        //       Do not write any test that does not follow this rule.
        // Note: Unsigned char has the range 0 - 255.
        [0]  = { .single_int = {EOF} },
        [1]  = { .single_int = {0  } },
        [2]  = { .single_int = {127} }, // DEL
        [3]  = { .single_int = {255} }, // Max unsigned char
        [4]  = { .single_int = {'0'    } }, // Inside
        [5]  = { .single_int = {'0' - 1} }, // Catch '/' (Outside)
        [6]  = { .single_int = {'9'    } }, // Inside
        [7]  = { .single_int = {'9' + 1} }, // Catch ':' (Outside)
        [8]  = { .single_int = {'A'    } }, // Inside
        [9]  = { .single_int = {'A' - 1} }, // Catch '@' (Outside)
        [10] = { .single_int = {'Z'    } }, // Inside
        [11] = { .single_int = {'Z' + 1} }, // Catch '[' (Outside)
        [12] = { .single_int = {'a'    } }, // Inside
        [13] = { .single_int = {'a' - 1} }, // Catch '`' (Outside)
        [14] = { .single_int = {'z'    } }, // Inside
        [15] = { .single_int = {'z' + 1} }, // Catch '{' (Outside)
        [16] = { .single_int = {' ' } },
        [17] = { .single_int = {'\n'} },
    },
    .test_count               = 18,
    .name                     = String8Literal("ft_isalnum"),
    .file                     = String8Literal(__FILE__),
    .function_return_type     = TestReturnType_Int,
    .function_parameters_type = TestParametersType_Int,
    .libft_function           = (void *)ft_isalnum,
    .callback                 = (TestCallbackFunction)callback_for_isalnum,
};
