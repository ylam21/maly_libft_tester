internal_function
TestPayload callback_for_strtrim(TestParameters test_parameters)
{
    TestPayload payload = {0};

    TestParameters_PtrPtrPtr parameters = test_parameters.ptr_ptr_ptr;

    char *expected_return = parameters.ptr3; // Note: Null-terminated string is expected.
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
    char *got_return = ft_strtrim((const char *)parameters.ptr1, (const char *)parameters.ptr2);
    if(got_return != 0)
    {
        U64 got_return_length = length_of_cstring(got_return) + 1;
        MemoryCopy(payload.buffer_for_got, got_return, got_return_length);
        payload.got_strings_sizes[0]    = got_return_length;
        payload.got_strings_sizes_count = 1;

        if((expected_return != 0) && (expected_return_length == got_return_length) && MemoryMatch(payload.buffer_for_got, payload.buffer_for_expected, expected_return_length))
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
void test_ft_strtrim(Tester *tester)
{
    TestParameters tests[] =
    {
        // 1. Basic & Multi-character Trims
        { .ptr_ptr_ptr = {__LINE__, "   Trim Me     ",          " ",         "Trim Me"} },
        { .ptr_ptr_ptr = {__LINE__, "xxxHello Worldyyy",        "xy",        "Hello World"} },
        { .ptr_ptr_ptr = {__LINE__, "ababaHello Worldbabab",    "ab",        "Hello World"} },

        // 2. One-Sided Trims
        { .ptr_ptr_ptr = {__LINE__, "     Left Trim",           " ",         "Left Trim"} },
        { .ptr_ptr_ptr = {__LINE__, "Right Trim     ",          " ",         "Right Trim"} },
        { .ptr_ptr_ptr = {__LINE__, "lorem ipsum dolor sit amet", "l",       "orem ipsum dolor sit amet"} },
        { .ptr_ptr_ptr = {__LINE__, "lorem ipsum dolor sit amet", "t",       "lorem ipsum dolor sit ame"} },

        // 3. Middle Protection (Must NOT trim from the inside)
        { .ptr_ptr_ptr = {__LINE__, "  Hello  World  ",         " ",         "Hello  World"} },
        { .ptr_ptr_ptr = {__LINE__, "xxxHelloxxWorldxxx",       "x",         "HelloxxWorld"} },

        // 4. Full Trims (CRITICAL: Pointers crossing over each other)
        { .ptr_ptr_ptr = {__LINE__, "          ",               " ",         ""} },
        { .ptr_ptr_ptr = {__LINE__, "ababababab",               "ab",        ""} },

        // 5. No Trimming Needed
        { .ptr_ptr_ptr = {__LINE__, "Hello World",              "xyz",       "Hello World"} },

        // 6. Empty Strings & Empty Sets (Must return malloc'd copy)
        { .ptr_ptr_ptr = {__LINE__, "",                         " ",         ""} },
        { .ptr_ptr_ptr = {__LINE__, "Hello World",              "",          "Hello World"} },
        { .ptr_ptr_ptr = {__LINE__, "",                         "",          ""} },

        // 7. Standard Whitespace/Control checks
        { .ptr_ptr_ptr = {__LINE__, " \t\n\v\f\rHello World \t\n\v\f\r", " \t\n\v\f\r", "Hello World"} },
    };
    U64 test_count = CountOfStaticArray(tests);

    // Save the current position in Tester's permanent arena and reset it only if tests were skipped.
    TemporaryArena temporary_arena = temporary_arena_begin(tester->permanent_arena);

    TestGroup test_group =
    {
        .name                      = String8Literal("ft_strtrim"),
        .file                      = string8_from_cstring(__FILE__),
        .failed_test_reports       = push_array(tester->permanent_arena, TestReport, test_count),
    };
    TestContext test_context =
    {
        .test_group     = test_group,

        .function_return_type     = TestReturnType_Ptr,
        .function_parameters_type = TestParametersType_PtrPtr,

        .tests          = tests,
        .test_count     = test_count,

        .libft_function = (void *)ft_strtrim,
        .callback       = (TestCallbackFunction)callback_for_strtrim,
    };

    run_tests(tester, &test_context);

    if(test_context.flags & TestContextFlag_TestsWereSkipped)
    {
        temporary_arena_end(temporary_arena);
    }
}
