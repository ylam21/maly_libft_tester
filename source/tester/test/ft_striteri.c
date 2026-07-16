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

    // Reset thread_static_allocation_count before calling libft function.
    thread_static_allocation_count = 0;

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

    if(!thread_static_allocation_count)
    {
        payload.flags |= TestPayloadFlag_NoMemoryLeak;
    }

    if((payload.flags & TestPayloadFlag_ResultsMatch) && (payload.flags & TestPayloadFlag_NoMemoryLeak))
    {
        payload.flags |= TestPayloadFlag_TestPassed;
    }

    payload.leak_count     = thread_static_allocation_count;
    payload.expected_value = (U64)parameters.ptr3;
    payload.got_value      = (U64)parameters.ptr1;

    return(payload);
}

read_only global TestGroup test_group_ft_striteri =
{
    .tests =
    {
        [0] = { .ptr_ptr_ptr = {"i want to be uppercase!", make_string_uppercase, "I WANT TO BE UPPERCASE!"} },
        [1] = { .ptr_ptr_ptr = {"Hello",                   replace_with_x,        "XXXXX"} },
        [2] = { .ptr_ptr_ptr = {"00000",                   add_index_to_char,     "01234"} },
        [3] = { .ptr_ptr_ptr = {"abcde",                   add_index_to_char,     "acegi"} },
        [4] = { .ptr_ptr_ptr = {"a",                       make_string_uppercase, "A"} },
        [5] = { .ptr_ptr_ptr = {" ",                       replace_with_x,        "X"} },
        [6] = { .ptr_ptr_ptr = {"",                        make_string_uppercase, ""} },
        [7] = { .ptr_ptr_ptr = {"",                        add_index_to_char,     ""} },
    },
    .test_count               = 7,
    .name                     = String8Literal("ft_striteri"),
    .file                     = String8Literal(__FILE__),
    .function_return_type     = TestReturnType_Void,
    .function_parameters_type = TestParametersType_PtrPtr,
    .libft_function           = (void *)ft_striteri,
    .callback                 = (TestCallbackFunction)callback_for_striteri,
};
