internal char add_index(unsigned int index, char character)
{
    return((char)(character + index));
}

internal char return_index_digit(unsigned int index, char character)
{
    (void)character;
    return((char)((index % 10) + '0'));
}

internal char always_x(unsigned int index, char character)
{
    (void)index;
    (void)character;
    return('X');
}

internal char upper_odd_characters(unsigned int index, char character)
{
    char result;

    result = character;
    if(index & 1)
    {
        result = upper_from_char(character);
    }

    return(result);
}

internal char lower_odd_characters(unsigned int index, char character)
{
    char result;

    result = character;
    if(index & 1)
    {
        result = lower_from_char(character);
    }

    return(result);
}

internal TestPayload callback_for_strmapi(TestParameters test_parameters)
{
    TestPayload payload = {0};

    TestParameters_PtrPtrPtr parameters = test_parameters.ptr_ptr_ptr;

    char *expected_return = (char *)parameters.ptr3;
    U64 expected_return_size;
    if(expected_return != 0)
    {
        expected_return_size = length_of_cstring(expected_return) + 1;
        MemoryCopy(payload.buffer_for_expected, expected_return, expected_return_size);
        payload.expected_strings_sizes[0]    = expected_return_size;
        payload.expected_strings_sizes_count = 1;
    }

    // Reset global_allocation_count before calling libft function.
    global_allocation_count = 0;

    // Call libft function and save the result.
    char *got_return = ft_strmapi((const char *)parameters.ptr1, (StrmapiFunction)parameters.ptr2);
    U64 got_return_size;
    if(got_return != 0)
    {
        got_return_size = length_of_cstring(got_return) + 1;
        MemoryCopy(payload.buffer_for_got, got_return, got_return_size);
        payload.got_strings_sizes[0]    = got_return_size;
        payload.got_strings_sizes_count = 1;

        if((expected_return != 0) &&
           (expected_return_size == got_return_size) &&
           MemoryMatch(payload.buffer_for_got, payload.buffer_for_expected, expected_return_size))
        {
            payload.flags |= TestPayloadFlag_ResultsMatch;
        }

        free(got_return);
    }
    else
    {
        if(expected_return == 0)
        {
            payload.flags |= TestPayloadFlag_ResultsMatch;
        }
    }

    payload.leak_count     = global_allocation_count;
    payload.expected_value = (U64)expected_return;
    payload.got_value      = (U64)got_return;

    return(payload);
}

internal void test_ft_strmapi(Tester *tester)
{
    TestParameters tests[] =
    {
        // 1. Basic Alternating Case (Tests your provided functions)
        { .ptr_ptr_ptr = {__LINE__, "very nice!", upper_odd_characters, "vErY NiCe!"} },
        { .ptr_ptr_ptr = {__LINE__, "VERY NICE!", lower_odd_characters, "VeRy nIcE!"} },
        { .ptr_ptr_ptr = {__LINE__, "abcdef",     upper_odd_characters, "aBcDeF"} },

        // 2. Index Verification (CRITICAL: Catches students who don't pass the right index)
        // '0'+0='0', '0'+1='1', '0'+2='2', etc.
        { .ptr_ptr_ptr = {__LINE__, "00000",      add_index,            "01234"} },
        // 'a'+0='a', 'b'+1='c', 'c'+2='e', 'd'+3='g', 'e'+4='i'
        { .ptr_ptr_ptr = {__LINE__, "abcde",      add_index,            "acegi"} },

        // Ignores the string entirely, just prints the index
        { .ptr_ptr_ptr = {__LINE__, "Hello",      return_index_digit,   "01234"} },
        { .ptr_ptr_ptr = {__LINE__, "42 Prague!", return_index_digit,   "0123456789"} },

        // 3. Complete Overwrites (Tests standard iteration bounds)
        { .ptr_ptr_ptr = {__LINE__, "Hello",      always_x,             "XXXXX"} },
        { .ptr_ptr_ptr = {__LINE__, " ",          always_x,             "X"} },

        // 4. Empty Strings (CRITICAL: Must return a valid malloc'd "")
        { .ptr_ptr_ptr = {__LINE__, "",           upper_odd_characters, ""} },
        { .ptr_ptr_ptr = {__LINE__, "",           always_x,             ""} },
        { .ptr_ptr_ptr = {__LINE__, "",           add_index,            ""} },

        // 5. Single Characters
        { .ptr_ptr_ptr = {__LINE__, "A",          upper_odd_characters, "A"} },
        { .ptr_ptr_ptr = {__LINE__, "a",          upper_odd_characters, "a"} },
    };
    U64 test_count = CountOfStaticArray(tests);

    // Save the current position in Tester's permanent arena and reset it only if tests were skipped.
    TemporaryArena temporary_arena = temporary_arena_begin(tester->permanent_arena);

    TestGroup test_group =
    {
        .name                      = String8Literal("ft_strmapi"),
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

        .libft_function = (void *)ft_strmapi,
        .callback       = (TestCallbackFunction)callback_for_strmapi,
    };

    run_tests(tester, &test_context);

    if(test_context.flags & TestContextFlag_TestsWereSkipped)
    {
        temporary_arena_end(temporary_arena);
    }
}
