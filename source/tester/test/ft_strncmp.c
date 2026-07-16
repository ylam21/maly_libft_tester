internal_function
TestPayload callback_for_strncmp(TestParameters test_parameters)
{
    TestPayload payload = {0};

    TestParameters_PtrPtrSize parameters = test_parameters.ptr_ptr_size;

    // Call libc function.
    int expected_return = strncmp((const char *)parameters.ptr1, (const char *)parameters.ptr2, (size_t)parameters.size);

    // Reset thread_static_allocation_count before calling libft function.
    thread_static_allocation_count = 0;

    // Call libft function.
    int got_return = ft_strncmp((const char *)parameters.ptr1, (const char *)parameters.ptr2, (size_t)parameters.size);

    U32 signs_match = 0;
    if (got_return == expected_return)
    {
        signs_match = 1;
    }
    else if ((got_return < 0 && expected_return < 0) || (got_return > 0 && expected_return > 0))
    {
        signs_match = 1;
    }

    if(signs_match)
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
    payload.expected_value = (U64)expected_return;
    payload.got_value      = (U64)got_return;

    return(payload);
}

read_only global TestGroup test_group_ft_strncmp =
{
    .tests =
    {
        [0]  = { .ptr_ptr_size = {(void *)"Hello", (void *)"World", 0} },
        [1]  = { .ptr_ptr_size = {(void *)"",      (void *)"",      0} },
        [2]  = { .ptr_ptr_size = {(void *)"Hello", (void *)"Hello", 5} },
        [3]  = { .ptr_ptr_size = {(void *)"Hello", (void *)"Hello", 10} }, // n > length
        [4]  = { .ptr_ptr_size = {(void *)"Hello World", (void *)"Hello Zorld", 5} },
        [5]  = { .ptr_ptr_size = {(void *)"Hello World", (void *)"Hello Zorld", 6} },
        [6]  = { .ptr_ptr_size = {(void *)"Hello Zorld", (void *)"Hello World", 6} },
        [7]  = { .ptr_ptr_size = {(void *)"Test",  (void *)"Testing", 10} },
        [8]  = { .ptr_ptr_size = {(void *)"Testing", (void *)"Test",  10} },
        [9]  = { .ptr_ptr_size = {(void *)"a\0b\0c", (void *)"a\0b\0x", 5} }, // Returns 0
        [10] = { .ptr_ptr_size = {(void *)"test\0x", (void *)"test\0y", 10} }, // Returns 0
        [11] = { .ptr_ptr_size = {(void *)"", (void *)"a", 1} },
        [12] = { .ptr_ptr_size = {(void *)"a", (void *)"", 1} },
        [13] = { .ptr_ptr_size = {(void *)"\x80", (void *)"\x00", 1} },
        [14] = { .ptr_ptr_size = {(void *)"\x00", (void *)"\x80", 1} },
        [15] = { .ptr_ptr_size = {(void *)"\xff\xaa", (void *)"\x00\xaa", 2} },
    },
    .test_count               = 16,
    .name                     = String8Literal("ft_strncmp"),
    .file                     = String8Literal(__FILE__),
    .function_return_type     = TestReturnType_Int,
    .function_parameters_type = TestParametersType_PtrPtrSize,
    .libft_function           = (void *)ft_strncmp,
    .callback                 = (TestCallbackFunction)callback_for_strncmp,
};
