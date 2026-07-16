internal_function
TestPayload callback_for_lstsize(TestParameters test_parameters)
{
    TestPayload payload = {0};
    TestParameters_Size parameters = test_parameters.size;

    int expected_size = (int)parameters.size;

    t_list *head = 0;
    for(int index = 0; index < expected_size; index += 1)
    {
        t_list *new_node  = (t_list *)malloc(sizeof(t_list));
        new_node->content = (void *)"Dummy Data";
        new_node->next    = head;
        head              = new_node;
    }

    // Reset thread_static_allocation_count before calling libft function.
    thread_static_allocation_count = 0;

    // Call the libft function.
    int got_size = ft_lstsize((t_list *)head);

    if(got_size == expected_size)
    {
        payload.flags |= TestPayloadFlag_ResultsMatch;
    }

    // Free the list.
    t_list *current = head;
    while (current != 0)
    {
        t_list *next = current->next;
        free(current);
        current = next;
    }

    if(!(thread_static_allocation_count > 0))
    {
        payload.flags |= TestPayloadFlag_NoMemoryLeak;
    }

    if((payload.flags & TestPayloadFlag_ResultsMatch) && (payload.flags & TestPayloadFlag_NoMemoryLeak))
    {
        payload.flags |= TestPayloadFlag_TestPassed;
    }

    payload.leak_count     = thread_static_allocation_count;
    payload.expected_value = (U64)expected_size;
    payload.got_value      = (U64)got_size;

    return(payload);
}

read_only global TestGroup test_group_ft_lstsize =
{
    .tests =
    {
        [0] = { .size = {0} },
        [1] = { .size = {1} },
        [2] = { .size = {2} },
        [3] = { .size = {5} },
        [4] = { .size = {42} },
    },
    .test_count               = 5,
    .name                     = String8Literal("ft_lstsize"),
    .file                     = String8Literal(__FILE__),
    .function_return_type     = TestReturnType_Int,
    .function_parameters_type = TestParametersType_Size,
    .libft_function           = (void *)ft_lstsize,
    .callback                 = (TestCallbackFunction)callback_for_lstsize,
};
