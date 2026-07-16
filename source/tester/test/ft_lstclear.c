internal_function
TestPayload callback_for_lstclear(TestParameters test_parameters)
{
    TestPayload payload = {0};
    int expected_size = (int)test_parameters.size.size;

    // Reset thread_static_allocation_count before calling libft function.
    thread_static_allocation_count = 0;
    // Reset fake_delete_call_count before calling libft function.
    fake_delete_call_count = 0;

    t_list *head = 0;
    for (int index = 0; index < expected_size; index += 1)
    {
        t_list *new_node = (t_list *)malloc(sizeof(t_list));
        new_node->content = (void *)"Delete Me";
        new_node->next = head;
        head = new_node;
    }

    char expected_string[64];
    int expected_length = snprintf(expected_string, sizeof(expected_string), "Deleted %d nodes, Head=NULL", expected_size);
    MemoryCopy(payload.buffer_for_expected, expected_string, expected_length + 1);
    payload.expected_strings_sizes[0] = expected_length + 1;
    payload.expected_strings_sizes_count = 1;

    // Call libft function.
    ft_lstclear((t_list **)&head, (LstdeloneFunction)fake_delete_function);

    char got_string[64];
    int got_length = snprintf(got_string, sizeof(got_string), "Deleted %d nodes, Head=%s",
                           fake_delete_call_count, head == 0 ? "NULL" : "NOT NULL");
    MemoryCopy(payload.buffer_for_got, got_string, got_length + 1);
    payload.got_strings_sizes[0] = got_length + 1;
    payload.got_strings_sizes_count = 1;

    if((fake_delete_call_count == expected_size) && (head == 0))
    {
        payload.flags |= TestPayloadFlag_ResultsMatch;
    }

    t_list *current = head;
    while(current != 0)
    {
        t_list *next = current->next;
        free(current);
        current = next;
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
    payload.expected_value = 0;
    payload.got_value      = 0;

    return(payload);
}

read_only global TestGroup test_group_ft_lstclear =
{
    .tests =
    {
        [0] = { .size = { 0} }, // The NULL trap (Empty list)
        [1] = { .size = { 1} }, // Single node
        [2] = { .size = { 3} }, // Standard list
        [3] = { .size = {10} }, // Stress test
    },
    .test_count               = 4,
    .name                     = String8Literal("ft_lstclear"),
    .file                     = String8Literal(__FILE__),
    .function_return_type     = TestReturnType_Void,
    .function_parameters_type = TestParametersType_Size,
    .libft_function           = (void *)ft_lstclear,
    .callback                 = (TestCallbackFunction)callback_for_lstclear,
};
