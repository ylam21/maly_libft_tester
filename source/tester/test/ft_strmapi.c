internal_function
char add_index(unsigned int index, char character)
{
    return((char)(character + index));
}

internal_function
char return_index_digit(unsigned int index, char character)
{
    (void)character;
    return((char)((index % 10) + '0'));
}

internal_function
char always_x(unsigned int index, char character)
{
    (void)index;
    (void)character;
    return('X');
}

internal_function
char upper_odd_characters(unsigned int index, char character)
{
    char result;

    result = character;
    if(index & 1)
    {
        result = upper_from_char(character);
    }

    return(result);
}

internal_function
char lower_odd_characters(unsigned int index, char character)
{
    char result;

    result = character;
    if(index & 1)
    {
        result = lower_from_char(character);
    }

    return(result);
}

internal_function
TestPayload callback_for_strmapi(TestParameters test_parameters)
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

    // Reset thread_static_allocation_count before calling libft function.
    thread_static_allocation_count = 0;

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

    if(!thread_static_allocation_count)
    {
        payload.flags |= TestPayloadFlag_NoMemoryLeak;
    }

    if((payload.flags & TestPayloadFlag_ResultsMatch) && (payload.flags & TestPayloadFlag_NoMemoryLeak))
    {
        payload.flags |= TestPayloadFlag_TestPassed;
    }

    payload.leak_count     = thread_static_allocation_count;
    payload.expected_value = (U64)expected_return;
    payload.got_value      = (U64)got_return;

    return(payload);
}

read_only global TestGroup test_group_ft_strmapi =
{
    .tests =
    {
        [0]  = { .ptr_ptr_ptr = {"very nice!", upper_odd_characters, "vErY NiCe!"} },
        [1]  = { .ptr_ptr_ptr = {"VERY NICE!", lower_odd_characters, "VeRy nIcE!"} },
        [2]  = { .ptr_ptr_ptr = {"abcdef",     upper_odd_characters, "aBcDeF"} },
        [3]  = { .ptr_ptr_ptr = {"00000",      add_index,            "01234"} },
        [4]  = { .ptr_ptr_ptr = {"abcde",      add_index,            "acegi"} },
        [5]  = { .ptr_ptr_ptr = {"Hello",      return_index_digit,   "01234"} },
        [6]  = { .ptr_ptr_ptr = {"42 Prague!", return_index_digit,   "0123456789"} },
        [7]  = { .ptr_ptr_ptr = {"Hello",      always_x,             "XXXXX"} },
        [8]  = { .ptr_ptr_ptr = {" ",          always_x,             "X"} },
        [9]  = { .ptr_ptr_ptr = {"",           upper_odd_characters, ""} },
        [10] = { .ptr_ptr_ptr = {"",           always_x,             ""} },
        [11] = { .ptr_ptr_ptr = {"",           add_index,            ""} },
        [12] = { .ptr_ptr_ptr = {"A",          upper_odd_characters, "A"} },
        [13] = { .ptr_ptr_ptr = {"a",          upper_odd_characters, "a"} },
    },
    .test_count               = 14,
    .name                     = String8Literal("ft_strmapi"),
    .file                     = String8Literal(__FILE__),
    .function_return_type     = TestReturnType_Ptr,
    .function_parameters_type = TestParametersType_PtrPtr,
    .libft_function           = (void *)ft_strmapi,
    .callback                 = (TestCallbackFunction)callback_for_strmapi,
};
