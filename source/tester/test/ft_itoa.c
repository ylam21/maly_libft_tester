internal_function
TestPayload callback_for_itoa(TestParameters test_parameters)
{
    TestPayload payload = {0};

    TestParameters_IntPtr parameters = test_parameters.int_ptr;

    char *expected_return = (char *)parameters.ptr; // Note: Null-terminated string is expected.
    U64 expected_return_length;
    if(expected_return != 0)
    {
        expected_return_length = length_of_cstring(expected_return) + 1;
        MemoryCopy(payload.buffer_for_expected, expected_return, expected_return_length);
        payload.expected_strings_sizes[0]    = expected_return_length;
        payload.expected_strings_sizes_count = 1;
    }

    // Reset global_allocation_count before calling libft function.
    global_allocation_count = 0;

    // Call libft function and save the result.
    char *got_return = ft_itoa((int)parameters.a);
    if(got_return != 0)
    {
        U64 got_return_length = length_of_cstring(got_return) + 1;
        MemoryCopy(payload.buffer_for_got, got_return, got_return_length);
        payload.got_strings_sizes[0]    = got_return_length;
        payload.got_strings_sizes_count = 1;

        if((expected_return != 0) && (expected_return_length == got_return_length) && MemoryMatch(got_return, expected_return, expected_return_length))
        {
            payload.flags |= TestPayloadFlag_ResultsMatch;
        }

        free(got_return);
    }
    else
    {
        if(expected_return == 0) // This should never be the case, but if it is, handle it.
        {
            payload.flags |= TestPayloadFlag_ResultsMatch;
        }
    }

    payload.leak_count     = global_allocation_count;
    payload.expected_value = (U64)expected_return;
    payload.got_value      = (U64)got_return;

    return(payload);
}

internal_function
void test_ft_itoa(Tester *tester)
{
    TestParameters tests[] =
    {
        // 1. Zero (Special case in most implementations)
        { .int_ptr = {__LINE__, 0,           "0"          } },

        // 2. Single Digits
        { .int_ptr = {__LINE__, 1,           "1"          } },
        { .int_ptr = {__LINE__, 9,           "9"          } },
        { .int_ptr = {__LINE__, -1,          "-1"         } },
        { .int_ptr = {__LINE__, -9,          "-9"         } },

        // 3. Double Digits & Powers of 10 (Catches reversing off-by-one bugs)
        { .int_ptr = {__LINE__, 10,          "10"         } },
        { .int_ptr = {__LINE__, -10,         "-10"        } },
        { .int_ptr = {__LINE__, 100,         "100"        } },
        { .int_ptr = {__LINE__, -100,        "-100"       } },

        // 4. Standard numbers
        { .int_ptr = {__LINE__, 42,          "42"         } },
        { .int_ptr = {__LINE__, -42,         "-42"        } },
        { .int_ptr = {__LINE__, 9999,        "9999"       } },
        { .int_ptr = {__LINE__, -9999,       "-9999"      } },
        { .int_ptr = {__LINE__, 1000000,     "1000000"    } },

        // 5. Integer Limits (CRITICAL)
        { .int_ptr = {__LINE__, 2147483647,  "2147483647" } }, // INT_MAX
        { .int_ptr = {__LINE__, -2147483647, "-2147483647"} }, // INT_MIN + 1
        { .int_ptr = {__LINE__, -2147483648, "-2147483648"} }, // INT_MIN (The ultimate trap)
    };
    U64 test_count = CountOfStaticArray(tests);

    // Save the current position in Tester's permanent arena and reset it only if tests were skipped.
    TemporaryArena temporary_arena = temporary_arena_begin(tester->permanent_arena);

    TestGroup test_group =
    {
        .name                      = String8Literal("ft_itoa"),
        .file                      = string8_from_cstring(__FILE__),
        .failed_test_reports       = push_array(tester->permanent_arena, TestReport, test_count),
    };
    TestContext test_context =
    {
        .test_group     = test_group,

        .function_return_type     = TestReturnType_Ptr,
        .function_parameters_type = TestParametersType_Int,

        .tests          = tests,
        .test_count     = test_count,

        .libft_function = (void *)ft_itoa,
        .callback       = (TestCallbackFunction)callback_for_itoa,
    };

    run_tests(tester, &test_context);

    if(test_context.flags & TestContextFlag_TestsWereSkipped)
    {
        temporary_arena_end(temporary_arena);
    }
}
