internal_function
TestPayload callback_for_isdigit(TestParameters test_parameters)
{
    TestPayload payload = {0};

    TestParameters_Int parameters = test_parameters.single_int;

    // Call libc function.
    int expected_return = isdigit((int)parameters.a);
    // Note from Libft subject (version 19.2):
    //    For the character classification functions (isalpha, isdigit, isalnum, isascii, isprint), the return value must be:
    //    • 1 if the character matches the tested class
    //    • 0 if the character does not match
    expected_return = MinimumBetween(expected_return, 1);

    // Reset thread_static_allocation_count before calling libft function.
    thread_static_allocation_count = 0;

    // Call libft function.
    int got_return = ft_isdigit((int)parameters.a);

    if(expected_return == got_return)
    {
        payload.flags |= TestPayloadFlag_ResultsMatch;
    }

    payload.leak_count     = thread_static_allocation_count;
    payload.expected_value = (U64)expected_return;
    payload.got_value      = (U64)got_return;

    return(payload);
}

read_only global TestGroup test_group_ft_isdigit =
{
    .tests =
    {
        // Note: isdigit argument 'c' must have the value of an unsigned char or EOF. (https://man7.org/linux/man-pages/man3/isspace.3.html)
        //       Do not write any test that does not follow this rule.
        // Note: Unsigned char has the range 0 - 255.
        [0]  = { .single_int = {EOF} },
        [1]  = { .single_int = {0  } },
        [2]  = { .single_int = {127} }, // DEL
        [3]  = { .single_int = {255} }, // Max unsigned char
        [4]  = { .single_int = {'0'    } }, // 48
        [5]  = { .single_int = {'0' - 1} }, // 47 '/' (Just below range)
        [6]  = { .single_int = {'9'    } }, // 57
        [7]  = { .single_int = {'9' + 1} }, // 58 ':' (Just above range)
        [8]  = { .single_int = {'4'    } }, // Middle of range
        [9]  = { .single_int = {1} }, // Integer 1 (ASCII SOH), NOT char '1'
        [10] = { .single_int = {9} }, // Integer 9 (ASCII TAB), NOT char '9'
        [11] = { .single_int = {'+'} },
        [12] = { .single_int = {'-'} },
        [13] = { .single_int = {'.'} },
        [14] = { .single_int = {'a' } },
        [15] = { .single_int = {'Z' } },
        [16] = { .single_int = {' ' } },
        [17] = { .single_int = {'\n'} },
    },
    .test_count               = 18,
    .name                     = String8Literal("ft_isdigit"),
    .file                     = String8Literal(__FILE__),
    .function_return_type     = TestReturnType_Int,
    .function_parameters_type = TestParametersType_Int,
    .libft_function           = (void *)ft_isdigit,
    .callback                 = (TestCallbackFunction)callback_for_isdigit,
};
