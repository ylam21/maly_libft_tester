internal_function
TestPayload callback_for_tolower(TestParameters test_parameters)
{
    TestPayload payload = {0};

    TestParameters_Int parameters = test_parameters.single_int;

    // Call libc function.
    int expected_return = tolower((int)parameters.a);

    // Reset global_allocation_count before calling libft function.
    global_allocation_count = 0;

    // Call libft function.
    int got_return = ft_tolower((int)parameters.a);

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
void test_ft_tolower(Tester *tester)
{
    TestParameters tests[] =
    {
        // Note: tolower argument 'c' must have the value of an unsigned char or EOF. (https://man7.org/linux/man-pages/man3/tolower.3p.html)
        //       Do not write any test that does not follow this rule.
        // Note: Unsigned char has the range 0 - 255.

        // Extremes
        { .single_int = {__LINE__, EOF} },
        { .single_int = {__LINE__, 0  } },
        { .single_int = {__LINE__, 255} }, // Max unsigned char

        // Uppercase Boundaries (CRITICAL: Must change to lowercase)
        { .single_int = {__LINE__, 'A'    } }, // 65 -> 97 ('a')
        { .single_int = {__LINE__, 'A' - 1} }, // 64 '@' -> Must remain unchanged
        { .single_int = {__LINE__, 'Z'    } }, // 90 -> 122 ('z')
        { .single_int = {__LINE__, 'Z' + 1} }, // 91 '[' -> Must remain unchanged

        // Mid-range Uppercase
        { .single_int = {__LINE__, 'M'    } },

        // Lowercase Boundaries (CRITICAL: Must remain unchanged)
        { .single_int = {__LINE__, 'a'    } },
        { .single_int = {__LINE__, 'a' - 1} }, // 96 '`' -> Must remain unchanged
        { .single_int = {__LINE__, 'z'    } },
        { .single_int = {__LINE__, 'z' + 1} }, // 123 '{' -> Must remain unchanged

        // Other non-alphabetic characters (Must remain unchanged)
        { .single_int = {__LINE__, '0' } },
        { .single_int = {__LINE__, '\n'} },
        { .single_int = {__LINE__, ' ' } },
    };
    U64 test_count = CountOfStaticArray(tests);

    // Save the current position in Tester's permanent arena and reset it only if tests were skipped.
    TemporaryArena temporary_arena = temporary_arena_begin(tester->permanent_arena);

    TestGroup test_group =
    {
        .name                      = String8Literal("ft_tolower"),
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

        .libft_function = (void *)ft_tolower,
        .callback       = (TestCallbackFunction)callback_for_tolower,
    };

    run_tests(tester, &test_context);

    if(test_context.flags & TestContextFlag_TestsWereSkipped)
    {
        temporary_arena_end(temporary_arena);
    }
}
