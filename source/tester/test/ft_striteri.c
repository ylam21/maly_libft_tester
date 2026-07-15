internal_function
void make_string_uppercase(unsigned int index, char *character)
{
    (void)index;
    *character = upper_from_char(*character);
}

internal_function
void replace_with_x(unsigned int index, char *character)
{
    (void)index;
    *character = 'X';
}

internal_function
void add_index_to_char(unsigned int index, char *character)
{
    *character = (char)(*character + index);
}

internal_function
TestPayload callback_for_striteri(TestParameters test_parameters)
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

    char mutable_string[TESTER_MAXIMUM_PAYLOAD_BUFFER_SIZE];
    U64 input_size = length_of_cstring((char *)parameters.ptr1) + 1;
    MemoryCopy(mutable_string, parameters.ptr1, input_size);

    // Reset global_allocation_count before calling libft function.
    global_allocation_count = 0;

    // Call libft function and save the result.
    ft_striteri(mutable_string, (StriteriFunction)parameters.ptr2);

    U64 got_return_size = length_of_cstring(mutable_string) + 1;
    MemoryCopy(payload.buffer_for_got, mutable_string, got_return_size);
    payload.got_strings_sizes[0]    = got_return_size;
    payload.got_strings_sizes_count = 1;

    if((expected_return != 0) &&
       (expected_return_size == got_return_size) &&
       MemoryMatch(payload.buffer_for_got, payload.buffer_for_expected, expected_return_size))
    {
        payload.flags |= TestPayloadFlag_ResultsMatch;
    }
    else if (expected_return == 0)
    {
        payload.flags |= TestPayloadFlag_ResultsMatch;
    }

    payload.leak_count     = global_allocation_count;
    payload.expected_value = (U64)parameters.ptr3;
    payload.got_value      = (U64)parameters.ptr1;

    return(payload);
}

internal_function
void test_ft_striteri(Tester *tester)
{
    TestParameters tests[] =
    {
        // 1. Standard iterations (Tests pointer modification)
        { .ptr_ptr_ptr = {__LINE__, "i want to be uppercase!", make_string_uppercase, "I WANT TO BE UPPERCASE!"} },
        { .ptr_ptr_ptr = {__LINE__, "Hello",                   replace_with_x,        "XXXXX"} },

        // 2. Index Verification (CRITICAL: Catches students who ignore the index)
        // '0'+0='0', '0'+1='1', '0'+2='2', etc.
        { .ptr_ptr_ptr = {__LINE__, "00000",                   add_index_to_char,     "01234"} },
        // 'a'+0='a', 'b'+1='c', 'c'+2='e', 'd'+3='g', 'e'+4='i'
        { .ptr_ptr_ptr = {__LINE__, "abcde",                   add_index_to_char,     "acegi"} },

        // 3. Single character bounds
        { .ptr_ptr_ptr = {__LINE__, "a",                       make_string_uppercase, "A"} },
        { .ptr_ptr_ptr = {__LINE__, " ",                       replace_with_x,        "X"} },

        // 4. Empty strings (CRITICAL: Must immediately return without doing anything)
        { .ptr_ptr_ptr = {__LINE__, "",                        make_string_uppercase, ""} },
        { .ptr_ptr_ptr = {__LINE__, "",                        add_index_to_char,     ""} },
    };
    U64 test_count = CountOfStaticArray(tests);

    // Save the current position in Tester's permanent arena and reset it only if tests were skipped.
    TemporaryArena temporary_arena = temporary_arena_begin(tester->permanent_arena);

    TestGroup test_group =
    {
        .name                      = String8Literal("ft_striteri"),
        .file                      = string8_from_cstring(__FILE__),
        .failed_test_reports       = push_array(tester->permanent_arena, TestReport, test_count),
    };
    TestContext test_context =
    {
        .test_group     = test_group,

        .function_return_type     = TestReturnType_Void,
        .function_parameters_type = TestParametersType_PtrPtr,

        .tests          = tests,
        .test_count     = test_count,

        .libft_function = (void *)ft_striteri,
        .callback       = (TestCallbackFunction)callback_for_striteri,
    };

    run_tests(tester, &test_context);

    if(test_context.flags & TestContextFlag_TestsWereSkipped)
    {
        temporary_arena_end(temporary_arena);
    }
}
