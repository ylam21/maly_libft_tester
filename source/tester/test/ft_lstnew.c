internal_function
TestPayload callback_for_lstnew(TestParameters test_parameters)
{
    TestPayload payload = {0};
    TestParameters_Ptr parameters = test_parameters.ptr;

    char *expected_content = (char *)parameters.ptr;
    if (expected_content != 0)
    {
        U64 expected_len = length_of_cstring(expected_content) + 1;
        MemoryCopy(payload.buffer_for_expected, expected_content, expected_len);
        payload.expected_strings_sizes[0]    = expected_len;
        payload.expected_strings_sizes_count = 1;
    }

    thread_static_allocation_count = 0;

    // Call libft function.
    t_list *got_node = ft_lstnew(parameters.ptr);

    if (got_node != 0)
    {
        void *got_content = got_node->content;
        void *got_next    = got_node->next;

        if (got_content != 0)
        {
            U64 got_len = length_of_cstring((char *)got_content) + 1;
            MemoryCopy(payload.buffer_for_got, got_content, got_len);
            payload.got_strings_sizes[0]    = got_len;
            payload.got_strings_sizes_count = 1;
        }

        if ((got_content == parameters.ptr) && (got_next == 0))
        {
            payload.flags |= TestPayloadFlag_ResultsMatch;
        }

        free(got_node);
    }
    else if (expected_content == 0)
    {
        payload.flags |= TestPayloadFlag_ResultsMatch;
    }

    payload.leak_count = thread_static_allocation_count;

    return(payload);
}

read_only global TestGroup test_group_ft_lstnew =
{
    .tests =
    {
        [0] = { .ptr = {"Hello World"} },
        [1] = { .ptr = {"42 Prague"} },
        [2] = { .ptr = {""} },
        [3] = { .ptr = {0} },
    },
    .test_count               = 4,
    .name                     = String8Literal("ft_lstnew"),
    .file                     = String8Literal(__FILE__),
    .function_return_type     = TestReturnType_Ptr,
    .function_parameters_type = TestParametersType_Ptr,
    .libft_function           = (void *)ft_lstnew,
    .callback                 = (TestCallbackFunction)callback_for_lstnew,
};
