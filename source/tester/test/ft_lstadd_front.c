internal_function
TestPayload callback_for_lstadd_front(TestParameters test_parameters)
{
    TestPayload payload = {0};
    TestParameters_PtrPtr parameters = test_parameters.ptr_ptr;

    char *expected_content = (char *)parameters.ptr2;
    if(expected_content != 0)
    {
        U64 expected_length = length_of_cstring(expected_content) + 1;
        MemoryCopy(payload.buffer_for_expected, expected_content, expected_length);
        payload.expected_strings_sizes[0]    = expected_length;
        payload.expected_strings_sizes_count = 1;
    }

    t_list *old_head = 0;
    if(parameters.ptr1 != 0)
    {
        old_head = ft_lstnew((void *)parameters.ptr1);
    }

    t_list *new_node = ft_lstnew((void *)parameters.ptr2);

    t_list *head = old_head;

    // Reset thread_static_allocation_count before calling libft function.
    thread_static_allocation_count = 0;

    // Call libft function.
    ft_lstadd_front((t_list **)&head, (t_list *)new_node);

    payload.leak_count = thread_static_allocation_count;

    if(!thread_static_allocation_count)
    {
        payload.flags |= TestPayloadFlag_NoMemoryLeak;
    }

    if(head != 0 && head->content != 0)
    {
        U64 got_length = length_of_cstring((char *)head->content) + 1;
        MemoryCopy(payload.buffer_for_got, head->content, got_length);
        payload.got_strings_sizes[0]    = got_length;
        payload.got_strings_sizes_count = 1;
    }

    if((head == new_node) && (head->next == old_head))
    {
        payload.flags |= TestPayloadFlag_ResultsMatch;
    }

    if(old_head != 0)
    {
        free(old_head);
    }
    if(new_node != 0)
    {
        free(new_node);
    }

    if((payload.flags & TestPayloadFlag_ResultsMatch) && (payload.flags & TestPayloadFlag_NoMemoryLeak))
    {
        payload.flags |= TestPayloadFlag_TestPassed;
    }

    payload.expected_value = 0;
    payload.got_value      = 0;

    return(payload);
}

read_only global TestGroup test_group_ft_lstadd_front =
{
    .tests =
    {
        // ptr1: Content of the existing list (0 means empty list)
        // ptr2: Content of the new node being added to the front
        [0] = { .ptr_ptr = {"Old Head", "New Head"} },
        [1] = { .ptr_ptr = {"World",    "Hello"} },
        [2] = { .ptr_ptr = {0,          "First Node"} },
        [3] = { .ptr_ptr = {"Old Head", ""} },
        [4] = { .ptr_ptr = {"",         "New Head"} },
    },
    .test_count               = 5,
    .name                     = String8Literal("ft_lstadd_front"),
    .file                     = String8Literal(__FILE__),
    .function_return_type     = TestReturnType_Void,
    .function_parameters_type = TestParametersType_PtrPtr,
    .libft_function           = (void *)ft_lstadd_front,
    .callback                 = (TestCallbackFunction)callback_for_lstadd_front,
};
