internal_function
TestPayload callback_for_memchr(TestParameters test_parameters)
{
    TestPayload payload = {0};

    TestParameters_PtrIntSize parameters = test_parameters.ptr_int_size;

    // Call libc function and save the result.
    void *expected_return = memchr((const void *)parameters.ptr, (int)parameters.a,(size_t)parameters.size);
    if(expected_return != 0)
    {
        U64 remaining_size = (U64)((U8*)parameters.ptr + parameters.size - (U8*)expected_return);
        MemoryCopy(payload.buffer_for_expected, expected_return, remaining_size);
        payload.expected_strings_sizes[0]    = remaining_size;
        payload.expected_strings_sizes_count = 1;
    }

    // Reset thread_static_allocation_count before calling libft function.
    thread_static_allocation_count = 0;

    // Call libft function and save the result.
    void *got_return = ft_memchr((const void *)parameters.ptr, (int)parameters.a,(size_t)parameters.size);
    if(got_return != 0)
    {
        U64 remaining_size = (U64)((U8*)parameters.ptr + parameters.size - (U8*)got_return);
        MemoryCopy(payload.buffer_for_got, got_return, remaining_size);
        payload.got_strings_sizes[0]    = remaining_size;
        payload.got_strings_sizes_count = 1;
    }

    if(got_return == expected_return)
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

read_only global TestGroup test_group_ft_memchr =
{
    .tests =
    {
        // Note: Passing NULL pointer to memchr results in undefined behaviour. Do not write such tests.
        // Note: memchr MUST cast the 'int' to 'unsigned char' internally.
        //       If ft_memchr compares using (char) some tests will be resolved as failed.
        [0]  = { .ptr_int_size = {"Hello", 'H',  6} },
        [1]  = { .ptr_int_size = {"Hello", 'l',  6} },
        [2]  = { .ptr_int_size = {"Hello", 'o',  6} },
        [3]  = { .ptr_int_size = {"Hello", 'z',  6} },
        [4]  = { .ptr_int_size = {"Hello", 'o',  2} },
        [5]  = { .ptr_int_size = {"Hello", 'H',  0} },
        [6]  = { .ptr_int_size = {"Hello", '\0', 6} },
        [7]  = { .ptr_int_size = {"Hello", '\0', 5} },
        [8]  = { .ptr_int_size = {"Find \xff the trap", -1,   15} }, // -1 cast to unsigned char is 255 (\xff)
        [9]  = { .ptr_int_size = {"Find \xff the trap", 255,  15} }, // Same as above, checking positive overflow
        [10] = { .ptr_int_size = {"Find \x80 the trap", -128, 15} }, // -128 cast to unsigned char is 128 (\x80)
        [11] = { .ptr_int_size = {"Hello", 'H' + 256, 6} }, // 'H' + 256 = 328. Cast to unsigned char, it wraps back to 72 ('H').
        [12] = { .ptr_int_size = {"42Prague", 'e', 8} }, // Target is the exact last byte allowed
        [13] = { .ptr_int_size = {"42Prague", 'e', 7} }, // Target is exactly one byte out of bounds (Should return NULL)
        [14] = { .ptr_int_size = {"42Prague", 'P', 1} }, // Size is 1, target is at index 2 (Should return NULL)
        [15] = { .ptr_int_size = {"a\0b\0c\0d", 'c', 8} }, // Must search past the first two null bytes
        [16] = { .ptr_int_size = {"a\0b\0c\0d", 'd', 8} }, // Must search past three null bytes
        [17] = { .ptr_int_size = {"a\0b\0c\0d", 'x', 8} }, // Should search the whole buffer and return NULL
        [18] = { .ptr_int_size = {"mississippi", 's', 12} },
        [19] = { .ptr_int_size = {"mississippi", 'p', 12} },
    },
    .test_count               = 20,
    .name                     = String8Literal("ft_memchr"),
    .file                     = String8Literal(__FILE__),
    .function_return_type     = TestReturnType_Ptr,
    .function_parameters_type = TestParametersType_PtrIntSize,
    .libft_function           = (void *)ft_memchr,
    .callback                 = (TestCallbackFunction)callback_for_memchr,
};
