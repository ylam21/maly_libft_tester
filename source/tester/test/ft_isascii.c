internal_function
TestPayload callback_for_isascii(TestParameters test_parameters)
{
    TestPayload payload = {0};

    TestParameters_Int parameters = test_parameters.single_int;

    // Call libc function.
    int expected_return = isascii((int)parameters.a);
    // Note from Libft subject (version 19.2):
    //    For the character classification functions (isalpha, isdigit, isalnum, isascii, isprint), the return value must be:
    //    • 1 if the character matches the tested class
    //    • 0 if the character does not match
    expected_return = MinimumBetween(expected_return, 1);

    // Reset global_allocation_count before calling libft function.
    global_allocation_count = 0;

    // Call libft function.
    int got_return = ft_isascii((int)parameters.a);

    if(expected_return == got_return)
    {
        payload.flags |= TestPayloadFlag_ResultsMatch;
    }

    payload.leak_count     = global_allocation_count;
    payload.expected_value = (U64)expected_return;
    payload.got_value      = (U64)got_return;

    return(payload);
}

internal_function
void test_ft_isascii(Tester *tester)
{
    TestParameters tests[] =
    {
        // Note: isascii argument 'c' must have the value of an unsigned char or EOF. (https://man7.org/linux/man-pages/man3/isspace.3.html)
        //       Do not write any test that does not follow this rule.
        // Note: Unsigned char has the range 0 - 255.
        // Note: isascii strictly checks for 7-bit ASCII range (0 to 127).

        // Extremes & The Core Boundaries (CRITICAL)
        { .single_int = {__LINE__, EOF} },
        { .single_int = {__LINE__, 0  } },     // Lowest valid ASCII
        { .single_int = {__LINE__, 127} },     // Highest valid ASCII
        { .single_int = {__LINE__, 128} },     // Lowest invalid ASCII (Just outside range)
        { .single_int = {__LINE__, 255} },     // Max unsigned char (Invalid)

        // Valid Mid-range characters
        { .single_int = {__LINE__, 'A' } },
        { .single_int = {__LINE__, 'a' } },
        { .single_int = {__LINE__, '0' } },
        { .single_int = {__LINE__, '\n'} },
        { .single_int = {__LINE__, ' ' } },
        { .single_int = {__LINE__, 64  } },    // '@'

        // Invalid Extended Range (Simulating extended ASCII/UTF-8 bytes)
        { .single_int = {__LINE__, 129 } },
        { .single_int = {__LINE__, 200 } },
        { .single_int = {__LINE__, 254 } },
    };
    U64 test_count = CountOfStaticArray(tests);

    // Save the current position in Tester's permanent arena and reset it only if tests were skipped.
    TemporaryArena temporary_arena = temporary_arena_begin(tester->permanent_arena);

    TestGroup test_group =
    {
        .name                      = String8Literal("ft_isascii"),
        .file                      = string8_from_cstring(__FILE__),
        .failed_test_reports       = push_array(tester->permanent_arena, TestReport, test_count),
    };
    TestContext test_context =
    {
        .test_group     = test_group,

        .function_return_type     = TestReturnType_Int,
        .function_parameters_type = TestParametersType_Int,

        .tests          = tests,
        .test_count     = test_count,

        .libft_function = (void *)ft_isascii,
        .callback       = (TestCallbackFunction)callback_for_isascii,
    };

    run_tests(tester, &test_context);

    if(test_context.flags & TestContextFlag_TestsWereSkipped)
    {
        temporary_arena_end(temporary_arena);
    }
}
