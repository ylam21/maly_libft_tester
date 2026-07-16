global int fake_delete_call_count = 0;

internal_function
void fake_delete_function(void *content)
{
    (void)content;
    fake_delete_call_count += 1;
}

internal_function
TestPayload callback_for_lstdelone(TestParameters test_parameters)
{
    TestPayload payload = {0};
    (void)test_parameters;

    t_list *node_1 = (t_list *)malloc(sizeof(t_list));
    node_1->content = (void *)"Delete Me";
    node_1->next    = (t_list *)0;

    t_list *node_2 = (t_list *)malloc(sizeof(t_list));
    node_2->content = (void *)"Dont Delete Me";
    node_2->next    = (t_list *)0;

    t_list *head = node_1;
    head->next   = node_2;

    // Reset fake_delete_call_count before calling libft function.
    fake_delete_call_count = 0;
    // Reset thread_static_allocation_count to 1 before calling libft function.
    // Libft function should free the head
    thread_static_allocation_count = 1;

    // Call libft function which should free also this node.
    ft_lstdelone((t_list *)head, (LstdeloneFunction)fake_delete_function);

    if((fake_delete_call_count == 1) && (node_2 != 0))
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
    payload.expected_value = 1;
    payload.got_value      = fake_delete_call_count;

    free(node_2);

    return(payload);
}

read_only global TestGroup test_group_ft_lstdelone =
{
    .test_count               = 1, // Pass void to the callback
    .name                     = String8Literal("ft_lstdelone"),
    .file                     = String8Literal(__FILE__),
    .function_return_type     = TestReturnType_Size,
    .function_parameters_type = TestParametersType_Void,
    .libft_function           = (void *)ft_lstdelone,
    .callback                 = (TestCallbackFunction)callback_for_lstdelone,
};
