internal TestPayload callback_for_strlen(TestParameters test_parameters)
{
    TestPayload payload = {0};

    TestParameters_Ptr parameters = test_parameters.ptr;

    // Call libc function.
    size_t expected_return = strlen((const char *)parameters.ptr);

    // Reset global_allocation_count before calling libft function.
    global_allocation_count = 0;

    // Call libft function.
    size_t got_return = ft_strlen((const char *)parameters.ptr);

    if(expected_return == got_return)
    {
        payload.flags |= TestPayloadFlag_ResultsMatch;
    }

    payload.leak_count     = global_allocation_count;
    payload.expected_value = (U64)expected_return;
    payload.got_value      = (U64)got_return;

    return(payload);
}

internal void test_ft_strlen(Tester *tester)
{
    TestParameters tests[] =
    {
        // 1. Empty and Single Characters
        { .ptr = {__LINE__, "" } },
        { .ptr = {__LINE__, "a"} },
        { .ptr = {__LINE__, " "} },
        { .ptr = {__LINE__, "0"} },

        // 2. Embedded Nulls (CRITICAL: strlen must stop at the first \0)
        // "Hello\0World" has an actual size of 12 in memory, but strlen MUST return 5.
        { .ptr = {__LINE__, "Hello\0World"} },
        { .ptr = {__LINE__, "\0Hidden"    } },
        { .ptr = {__LINE__, "a\0b\0c"     } },

        // 3. Normal Strings
        { .ptr = {__LINE__, "Hello, 42!"  } },
        { .ptr = {__LINE__, "123456"      } },

        // 4. Special Characters & Escape Traps
        { .ptr = {__LINE__, "\a\b\t\n\v\f\r"} }, // True control characters
        { .ptr = {__LINE__, "\\0"           } }, // Literal backslash-zero (len 2)
        { .ptr = {__LINE__, "/0"            } }, // Literal forward slash-zero (len 2)

        // 5. Extended/UTF-8 bytes (strlen counts BYTES, not visible characters)
        // The copyright symbol '©' is 2 bytes in UTF-8. strlen should count it as 2.
        { .ptr = {__LINE__, "©®™"         } },
        { .ptr = {__LINE__, "\xFF\xAA\x01"} },   // Arbitrary non-ascii hex bytes

        // 6. Long string
        { .ptr = {__LINE__, "A very long string... A very long string...  A very long string...  A very long string...  A very long string...  A very long string...  A very long string...  A very long string...  A very long string...  A very long string...  A very long string...  A very long string...  A very long string...  A very long string...  A very long string...  A very long string...   "} },
    };
    U64 test_count = CountOfStaticArray(tests);

    // Save the current position in Tester's permanent arena and reset it only if tests were skipped.
    TemporaryArena temporary_arena = temporary_arena_begin(tester->permanent_arena);

    TestGroup test_group =
    {
        .name                      = String8Literal("ft_strlen"),
        .file                      = string8_from_cstring(__FILE__),
        .failed_test_reports       = push_array(tester->permanent_arena, TestReport, test_count),
    };
    TestContext test_context =
    {
        .test_group     = test_group,

        .function_return_type     = TestReturnType_Size,
        .function_parameters_type = TestParametersType_Ptr,

        .tests          = tests,
        .test_count     = test_count,

        .libft_function = (void *)ft_strlen,
        .callback       = (TestCallbackFunction)callback_for_strlen,
    };

    run_tests(tester, &test_context);

    if(test_context.flags & TestContextFlag_TestsWereSkipped)
    {
        temporary_arena_end(temporary_arena);
    }
}
