internal_function
TestPayload callback_for_isprint(TestParameters test_parameters)
{
    TestPayload payload = {0};

    TestParameters_Int parameters = test_parameters.single_int;

    // Call libc function.
    int expected_return = isprint((int)parameters.a);
    // Note from Libft subject (version 19.2):
    //    For the character classification functions (isalpha, isdigit, isalnum, isascii, isprint), the return value must be:
    //    • 1 if the character matches the tested class
    //    • 0 if the character does not match
    expected_return = MinimumBetween(expected_return, 1);

    // Reset thread_static_allocation_count before calling libft function.
    thread_static_allocation_count = 0;

    // Call libft function.
    int got_return = ft_isprint((int)parameters.a);

    if(expected_return == got_return)
    {
        payload.flags |= TestPayloadFlag_ResultsMatch;
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

read_only global TestGroup test_group_ft_isprint =
{
    .tests =
    {
        // Note: isprint argument 'c' must have the value of an unsigned char or EOF. (https://man7.org/linux/man-pages/man3/isspace.3.html)
        //       Do not write any test that does not follow this rule.
        // Note: Unsigned char has the range 0 - 255.
        [0]  = { .single_int = {EOF} },
        [1]  = { .single_int = {0  } },     // NUL (Non-printable)
        [2]  = { .single_int = {255} },     // Max unsigned char (Non-printable)
        [3]  = { .single_int = {31 } },     // Unit Separator (Just below range)
        [4]  = { .single_int = {' '} },     // 32: Space (CRITICAL: Lowest printable)
        [5]  = { .single_int = {'!'} },     // 33: Exclamation (Printable)
        [6]  = { .single_int = {'}'} },     // 125: Right brace (Printable)
        [7]  = { .single_int = {'~'} },     // 126: Tilde (CRITICAL: Highest printable)
        [8]  = { .single_int = {127} },     // 127: DEL (Just above range, Non-printable)
        [9]  = { .single_int = {128} },     // Extended ASCII (Non-printable)
        [10] = { .single_int = {'\t'} },    // 9: Tab
        [11] = { .single_int = {'\n'} },    // 10: Newline
        [12] = { .single_int = {'\r'} },    // 13: Carriage return
        [13] = { .single_int = {'A'} },
        [14] = { .single_int = {'m'} },
        [15] = { .single_int = {'7'} },
    },
    .test_count               = 16,
    .name                     = String8Literal("ft_isprint"),
    .file                     = String8Literal(__FILE__),
    .function_return_type     = TestReturnType_Int,
    .function_parameters_type = TestParametersType_Int,
    .libft_function           = (void *)ft_isprint,
    .callback                 = (TestCallbackFunction)callback_for_isprint,
};
