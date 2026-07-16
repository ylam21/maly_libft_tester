internal_function
TestPayload callback_for_lstlast(TestParameters test_parameters)
{
    TestPayload payload = {0};
    TestParameters_Size parameters = test_parameters.size;

    int list_size = (int)parameters.size;

    t_list *head = 0;
    t_list *expected_last_node = 0;

    for(int index = 0; index < list_size; index += 1)
    {
        t_list *new_node = (t_list *)malloc(sizeof(t_list));

        if(index == 0)
        {
            new_node->content = (void *)"Target Last Node";
            expected_last_node = new_node;
        }
        else
        {
            new_node->content = (void *)"Standard Node";
        }

        new_node->next = head;
        head = new_node;
    }

    char *expected_string = expected_last_node ? (char *)expected_last_node->content : "NULL (Empty List)";
    U64 expected_length = length_of_cstring(expected_string) + 1;
    MemoryCopy(payload.buffer_for_expected, expected_string, expected_length);
    payload.expected_strings_sizes[0]    = expected_length;
    payload.expected_strings_sizes_count = 1;

    // Reset thread_static_allocation_count before calling libft function.
    thread_static_allocation_count = 0;

    // Call the libft function.
    t_list *got_last_node = ft_lstlast((t_list *)head);

    char *got_string = got_last_node ? (char *)got_last_node->content : "NULL (Empty List)";
    U64 got_length = length_of_cstring(got_string) + 1;
    MemoryCopy(payload.buffer_for_got, got_string, got_length);
    payload.got_strings_sizes[0]    = got_length;
    payload.got_strings_sizes_count = 1;

    if(got_last_node == expected_last_node)
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
    payload.expected_value = 0;
    payload.got_value      = 0;

    return(payload);
}

read_only global TestGroup test_group_ft_lstlast =
{
    .tests =
    {
        [0] = { .size = {0} },
        [1] = { .size = {1} },
        [2] = { .size = {2} },
        [3] = { .size = {5} },
        [4] = { .size = {4} },
    },
    .test_count               = 5,
    .name                     = String8Literal("ft_lstlast"),
    .file                     = String8Literal(__FILE__),
    .function_return_type     = TestReturnType_Ptr,
    .function_parameters_type = TestParametersType_Size,
    .libft_function           = (void *)ft_lstlast,
    .callback                 = (TestCallbackFunction)callback_for_lstlast,
};
