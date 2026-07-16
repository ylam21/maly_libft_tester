internal_function
TestPayload callback_for_atoi(TestParameters test_parameters)
{
    TestPayload payload = {0};

    TestParameters_Ptr parameters = test_parameters.ptr;

    // Call libc function.
    int expected_return = atoi((const char *)parameters.ptr);

    // Reset thread_static_allocation_count before calling libft function.
    thread_static_allocation_count = 0;

    // Call libft function.
    int got_return = ft_atoi((const char *)parameters.ptr);

    if(expected_return == got_return)
    {
        payload.flags |= TestPayloadFlag_ResultsMatch;
    }

    payload.leak_count     = thread_static_allocation_count;
    payload.expected_value = (U64)expected_return;
    payload.got_value      = (U64)got_return;

    return(payload);
}

read_only global TestGroup test_group_ft_atoi =
{
    .tests =
    {
        [0]  = { .ptr = {"0"             } },
        [1]  = { .ptr = {"-0"            } },
        [2]  = { .ptr = {"+0"            } },
        [3]  = { .ptr = {"42"            } },
        [4]  = { .ptr = {"-42"           } },
        [5]  = { .ptr = {"0000042"       } }, // Leading zeros
        [6]  = { .ptr = {" \t\n\v\f\r 42"} }, // All standard whitespaces
        [7]  = { .ptr = {"    -42"       } },
        [8]  = { .ptr = {"42abc"         } },
        [9]  = { .ptr = {"42  "          } },
        [10] = { .ptr = {"-42-3"         } }, // Stops at second minus
        [11] = { .ptr = {"--42"          } },
        [12] = { .ptr = {"++42"          } },
        [13] = { .ptr = {"+-42"          } },
        [14] = { .ptr = {" - 42"         } }, // Space after sign
        [15] = { .ptr = {""              } }, // Empty string
        [16] = { .ptr = {"+"             } }, // Only sign
        [17] = { .ptr = {"a42"           } }, // Letter before number
        [18] = { .ptr = {"2147483647"    } }, // INT_MAX
        [19] = { .ptr = {"-2147483648"   } }, // INT_MIN
    },
    .test_count               = 20,
    .name                     = String8Literal("ft_atoi"),
    .file                     = String8Literal(__FILE__),
    .function_return_type     = TestReturnType_Int,
    .function_parameters_type = TestParametersType_Ptr,
    .libft_function           = (void *)ft_atoi,
    .callback                 = (TestCallbackFunction)callback_for_atoi,
};
