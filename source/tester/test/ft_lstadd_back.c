internal TestPayload callback_for_lstadd_back(TestParameters test_parameters)
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

    // Reset global_allocation_count before calling libft function.
    global_allocation_count = 0;

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

    payload.leak_count     = global_allocation_count;
    payload.expected_value = 0;
    payload.got_value      = 0;

    return(payload);
}

internal void test_ft_lstadd_back(Tester *tester)
{
    TestParameters tests[] =
    {
        // 1. The Empty List Trap
        // Passes an empty list. Head must safely become new_node.
        { .size = {__LINE__, 0} },

        // 2. Standard Append Cases
        { .size = {__LINE__, 1} }, // Appending to a 1-node list
        { .size = {__LINE__, 2} },

        // 3. Stress Test
        { .size = {__LINE__, 1} },
    };
    U64 test_count = CountOfStaticArray(tests);

    TemporaryArena temporary_arena = temporary_arena_begin(tester->permanent_arena);

    TestGroup test_group =
    {
        .name                      = String8Literal("ft_lstadd_back"),
        .file                      = string8_from_cstring(__FILE__),
        .failed_test_reports       = push_array(tester->permanent_arena, TestReport, test_count),
    };

    TestContext test_context =
    {
        .test_group               = test_group,

        .function_return_type     = TestReturnType_Void,
        .function_parameters_type = TestParametersType_Size,

        .tests                    = tests,
        .test_count               = test_count,

        .libft_function           = (void *)ft_lstadd_back,
        .callback                 = (TestCallbackFunction)callback_for_lstadd_back,
    };

    run_tests(tester, &test_context);

    if(test_context.flags & TestContextFlag_TestsWereSkipped)
    {
        temporary_arena_end(temporary_arena);
    }
}
