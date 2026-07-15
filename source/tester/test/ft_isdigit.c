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

    // Reset global_allocation_count before calling libft function.
    global_allocation_count = 0;

    // Call libft function.
    int got_return = ft_isdigit((int)parameters.a);

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
void test_ft_isdigit(Tester *tester)
{
    TestParameters tests[] =
    {
        // Note: isdigit argument 'c' must have the value of an unsigned char or EOF. (https://man7.org/linux/man-pages/man3/isspace.3.html)
        //       Do not write any test that does not follow this rule.
        // Note: Unsigned char has the range 0 - 255.

        // Extremes & Basics
        { .single_int = {__LINE__, EOF} },
        { .single_int = {__LINE__, 0  } },
        { .single_int = {__LINE__, 127} }, // DEL
        { .single_int = {__LINE__, 255} }, // Max unsigned char

        // The Digit Boundaries
        { .single_int = {__LINE__, '0'    } }, // 48
        { .single_int = {__LINE__, '0' - 1} }, // 47 '/' (Just below range)
        { .single_int = {__LINE__, '9'    } }, // 57
        { .single_int = {__LINE__, '9' + 1} }, // 58 ':' (Just above range)
        { .single_int = {__LINE__, '4'    } }, // Middle of range

        // The "Value vs. Character" Trap
        { .single_int = {__LINE__, 1} }, // Integer 1 (ASCII SOH), NOT char '1'
        { .single_int = {__LINE__, 9} }, // Integer 9 (ASCII TAB), NOT char '9'

        // Number signs/symbols
        { .single_int = {__LINE__, '+'} },
        { .single_int = {__LINE__, '-'} },
        { .single_int = {__LINE__, '.'} },

        // Random Alphas & Whitespace
        { .single_int = {__LINE__, 'a' } },
        { .single_int = {__LINE__, 'Z' } },
        { .single_int = {__LINE__, ' ' } },
        { .single_int = {__LINE__, '\n'} },
    };
    U64 test_count = CountOfStaticArray(tests);

    // Save the current position in Tester's permanent arena and reset it only if tests were skipped.
    TemporaryArena temporary_arena = temporary_arena_begin(tester->permanent_arena);

    TestGroup test_group =
    {
        .name                      = String8Literal("ft_isdigit"),
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

        .libft_function = (void *)ft_isdigit,
        .callback       = (TestCallbackFunction)callback_for_isdigit,
    };

    run_tests(tester, &test_context);

    if(test_context.flags & TestContextFlag_TestsWereSkipped)
    {
        temporary_arena_end(temporary_arena);
    }
}
