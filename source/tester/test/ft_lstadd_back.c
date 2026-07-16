internal_function
TestPayload callback_for_lstadd_back(TestParameters test_parameters)
{
    TestPayload payload = {0};
    int list_size = (int)test_parameters.size.size;

    t_list *head = 0;
    t_list *old_last = 0;

    for(int index = 0; index < list_size; index += 1)
    {
        t_list *node = (t_list *)malloc(sizeof(t_list));
        node->content = (void *)"Standard Node";
        node->next = head;
        head = node;

        if(index == 0)
        {
            old_last = node;
        }
    }
    t_list *old_head = head;

    char *expected_content = "New Back Node";
    t_list *new_node  = (t_list *)malloc(sizeof(t_list));
    new_node->content = (void *)expected_content;
    new_node->next = 0;

    U64 expected_length = length_of_cstring(expected_content) + 1;
    MemoryCopy(payload.buffer_for_expected, expected_content, expected_length);
    payload.expected_strings_sizes[0]    = expected_length;
    payload.expected_strings_sizes_count = 1;

    // Reset thread_static_allocation_count before calling libft function.
    thread_static_allocation_count = 0;

    // Call libft function.
    ft_lstadd_back((t_list **)&head, (t_list *)new_node);

    t_list *got_last = head;
    if(got_last != 0)
    {
        while(got_last->next != 0)
        {
            got_last = got_last->next;
        }
    }

    char *got_string = got_last ? (char *)got_last->content : "NULL (Empty List)";
    U64 got_length   = length_of_cstring(got_string) + 1;
    MemoryCopy(payload.buffer_for_got, got_string, got_length);
    payload.got_strings_sizes[0]    = got_length;
    payload.got_strings_sizes_count = 1;

    int topology_is_correct = 0;
    if(list_size == 0)
    {
        // For an empty list, head MUST be updated to new_node
        if(head == new_node)
        {
            topology_is_correct = 1;
        }
    }
    else
    {
        // For a populated list, head must NOT change, and old_last->next MUST be new_node
        if((head == old_head) && (old_last->next == new_node))
        {
            topology_is_correct = 1;
        }
    }

    if(topology_is_correct)
    {
        payload.flags |= TestPayloadFlag_ResultsMatch;
    }

    // Free the list.
    int new_node_was_attached = 0;
    t_list *current = head;
    while(current != 0)
    {
        t_list *next = current->next;
        if(current == new_node)
        {
            new_node_was_attached = 1;
        }
        free(current);
        current = next;
    }

    if(new_node_was_attached == 0)
    {
        free(new_node);
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
    payload.expected_value = 0;
    payload.got_value      = 0;

    return(payload);
}

read_only global TestGroup test_group_ft_lstadd_back =
{
    .tests =
    {
        [0] = { .size = {0} },
        [1] = { .size = {1} }, // Appending to a 1-node list
        [2] = { .size = {2} },
        [3] = { .size = {3} },
    },
    .test_count               = 4,
    .name                     = String8Literal("ft_lstadd_back"),
    .file                     = String8Literal(__FILE__),
    .function_return_type     = TestReturnType_Void,
    .function_parameters_type = TestParametersType_Size,
    .libft_function           = (void *)ft_lstadd_back,
    .callback                 = (TestCallbackFunction)callback_for_lstadd_back,
};
