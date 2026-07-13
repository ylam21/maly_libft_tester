internal TestPayload callback_for_toupper(TestParameters test_parameters)
{
    TestPayload payload = {0};

    TestParameters_Int parameters = test_parameters.single_int;

    // Call libc function.
    int expected_return = toupper((int)parameters.a);

    // Reset global_allocation_count before calling libft function.
    global_allocation_count = 0;

    // Call libft function.
    int got_return = ft_toupper((int)parameters.a);

    if(expected_return == got_return)
    {
        payload.flags |= TestPayloadFlag_ResultsMatch;
    }

    payload.leak_count     = global_allocation_count;
    payload.expected_value = (U64)expected_return;
    payload.got_value      = (U64)got_return;

    return(payload);
}

internal void test_ft_toupper(Tester *tester)
{
    TestParameters tests[] =
    {
        // Note: toupper argument 'c' must have the value of an unsigned char or EOF. (https://linux.die.net/man/3/toupper)
        //       Do not write any test that does not follow this rule.
        // Note: Unsigned char has the range 0 - 255.

        // Extremes
        { .single_int = {__LINE__, EOF} },
        { .single_int = {__LINE__, 0  } },
        { .single_int = {__LINE__, 255} }, // Max unsigned char

        // Lowercase Boundaries (CRITICAL: Must change to uppercase)
        { .single_int = {__LINE__, 'a'    } }, // 97 -> 65 ('A')
        { .single_int = {__LINE__, 'a' - 1} }, // 96 '`' -> Must remain unchanged
        { .single_int = {__LINE__, 'z'    } }, // 122 -> 90 ('Z')
        { .single_int = {__LINE__, 'z' + 1} }, // 123 '{' -> Must remain unchanged

        // Mid-range Lowercase
        { .single_int = {__LINE__, 'm'    } },

        // Uppercase Boundaries (CRITICAL: Must remain unchanged)
        { .single_int = {__LINE__, 'A'    } },
        { .single_int = {__LINE__, 'A' - 1} }, // 64 '@' -> Must remain unchanged
        { .single_int = {__LINE__, 'Z'    } },
        { .single_int = {__LINE__, 'Z' + 1} }, // 91 '[' -> Must remain unchanged

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
        .name                      = String8Literal("ft_toupper"),
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

        .libft_function = (void *)ft_toupper,
        .callback       = (TestCallbackFunction)callback_for_toupper,
    };

    run_tests(tester, &test_context);

    if(test_context.flags & TestContextFlag_TestsWereSkipped)
    {
        temporary_arena_end(temporary_arena);
    }
}
