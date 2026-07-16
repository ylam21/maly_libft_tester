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

    // Reset thread_static_allocation_count before calling libft function.
    thread_static_allocation_count = 0;

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

read_only global TestGroup test_group_ft_itoa =
{
    .tests =
    {
        [0]  = { .int_ptr = {0,           "0"          } },
        [1]  = { .int_ptr = {1,           "1"          } },
        [2]  = { .int_ptr = {9,           "9"          } },
        [3]  = { .int_ptr = {-1,          "-1"         } },
        [4]  = { .int_ptr = {-9,          "-9"         } },
        [5]  = { .int_ptr = {10,          "10"         } },
        [6]  = { .int_ptr = {-10,         "-10"        } },
        [7]  = { .int_ptr = {100,         "100"        } },
        [8]  = { .int_ptr = {-100,        "-100"       } },
        [9]  = { .int_ptr = {42,          "42"         } },
        [10] = { .int_ptr = {-42,         "-42"        } },
        [11] = { .int_ptr = {9999,        "9999"       } },
        [12] = { .int_ptr = {-9999,       "-9999"      } },
        [13] = { .int_ptr = {1000000,     "1000000"    } },
        [14] = { .int_ptr = {2147483647,  "2147483647" } }, // INT_MAX
        [15] = { .int_ptr = {-2147483647, "-2147483647"} }, // INT_MIN + 1
        [16] = { .int_ptr = {-2147483648, "-2147483648"} }, // INT_MIN (The ultimate trap)
    },
    .test_count               = 17,
    .name                     = String8Literal("ft_itoa"),
    .file                     = String8Literal(__FILE__),
    .function_return_type     = TestReturnType_Ptr,
    .function_parameters_type = TestParametersType_Int,
    .libft_function           = (void *)ft_itoa,
    .callback                 = (TestCallbackFunction)callback_for_itoa,
};
