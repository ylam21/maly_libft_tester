internal_function
TestPayload callback_for_atoi(TestParameters test_parameters)
{
    TestPayload payload = {0};

    TestParameters_Ptr parameters = test_parameters.ptr;

    // Call libc function.
    int expected_return = atoi((const char *)parameters.ptr);

    // Reset global_allocation_count before calling libft function.
    global_allocation_count = 0;

    // Call libft function.
    int got_return = ft_atoi((const char *)parameters.ptr);

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
void test_ft_atoi(Tester *tester)
{
    TestParameters tests[] =
    {
        // 1. Basics & Zeros
        { .ptr = {__LINE__, "0"             } },
        { .ptr = {__LINE__, "-0"            } },
        { .ptr = {__LINE__, "+0"            } },

        // 2. Standard valid numbers
        { .ptr = {__LINE__, "42"            } },
        { .ptr = {__LINE__, "-42"           } },
        { .ptr = {__LINE__, "0000042"       } }, // Leading zeros

        // 3. Whitespace combinations
        { .ptr = {__LINE__, " \t\n\v\f\r 42"} }, // All standard whitespaces
        { .ptr = {__LINE__, "    -42"       } },

        // 4. Trailing garbage (should stop parsing)
        { .ptr = {__LINE__, "42abc"         } },
        { .ptr = {__LINE__, "42  "          } },
        { .ptr = {__LINE__, "-42-3"         } }, // Stops at second minus

        // 5. Invalid sign usage (CRITICAL: should return 0)
        { .ptr = {__LINE__, "--42"          } },
        { .ptr = {__LINE__, "++42"          } },
        { .ptr = {__LINE__, "+-42"          } },
        { .ptr = {__LINE__, " - 42"         } }, // Space after sign

        // 6. Edge cases and empty/incomplete strings
        { .ptr = {__LINE__, ""              } }, // Empty string
        { .ptr = {__LINE__, "+"             } }, // Only sign
        { .ptr = {__LINE__, "a42"           } }, // Letter before number

        // 7. Integer Limits (CRITICAL)
        // Note: atoi behavior on numbers larger than INT_MAX/INT_MIN is technically
        // undefined in standard C, so testing the exact boundaries is the safest bet.
        { .ptr = {__LINE__, "2147483647"    } }, // INT_MAX
        { .ptr = {__LINE__, "-2147483648"   } }, // INT_MIN
    };
    U64 test_count = CountOfStaticArray(tests);

    // Save the current position in Tester's permanent arena and reset it only if tests were skipped.
    TemporaryArena temporary_arena = temporary_arena_begin(tester->permanent_arena);

    TestGroup test_group =
    {
        .name                      = String8Literal("ft_atoi"),
        .file                      = string8_from_cstring(__FILE__),
        .failed_test_reports       = push_array(tester->permanent_arena, TestReport, test_count),
    };
    TestContext test_context =
    {
        .test_group     = test_group,

        .function_return_type     = TestReturnType_Int,
        .function_parameters_type = TestParametersType_Ptr,

        .tests          = tests,
        .test_count     = test_count,

        .libft_function = (void *)ft_atoi,
        .callback       = (TestCallbackFunction)callback_for_atoi,
    };

    run_tests(tester, &test_context);

    if(test_context.flags & TestContextFlag_TestsWereSkipped)
    {
        temporary_arena_end(temporary_arena);
    }
}
