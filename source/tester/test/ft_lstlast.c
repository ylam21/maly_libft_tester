internal TestPayload callback_for_lstlast(TestParameters test_parameters)
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

    // Reset global_allocation_count before calling libft function.
    global_allocation_count = 0;

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

    payload.leak_count     = global_allocation_count;
    payload.expected_value = 0;
    payload.got_value      = 0;

    return(payload);
}

internal void test_ft_lstlast(Tester *tester)
{
    TestParameters tests[] =
    {
        // 1. The Empty List Trap
        { .size = {__LINE__, 0} },

        // 2. Single Node
        { .size = {__LINE__, 1} },

        // 3. Standard Cases
        { .size = {__LINE__, 2} },
        { .size = {__LINE__, 5} },

        // 4. Stress Test
        { .size = {__LINE__, 4} },
    };
    U64 test_count = CountOfStaticArray(tests);

    TemporaryArena temporary_arena = temporary_arena_begin(tester->permanent_arena);

    TestGroup test_group =
    {
        .name                      = String8Literal("ft_lstlast"),
        .file                      = string8_from_cstring(__FILE__),
        .failed_test_reports       = push_array(tester->permanent_arena, TestReport, test_count),
    };

    TestContext test_context =
    {
        .test_group               = test_group,

        .function_return_type     = TestReturnType_Ptr,
        .function_parameters_type = TestParametersType_Size,

        .tests                    = tests,
        .test_count               = test_count,

        .libft_function           = (void *)ft_lstlast,
        .callback                 = (TestCallbackFunction)callback_for_lstlast,
    };

    run_tests(tester, &test_context);

    if(test_context.flags & TestContextFlag_TestsWereSkipped)
    {
        temporary_arena_end(temporary_arena);
    }
}
