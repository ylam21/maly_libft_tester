internal_function
TestPayload callback_for_lstclear(TestParameters test_parameters)
{
    TestPayload payload = {0};
    int expected_size = (int)test_parameters.size.size;

    // Reset global_allocation_count before calling libft function.
    global_allocation_count = 0;
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

    payload.leak_count     = global_allocation_count;
    payload.expected_value = 0;
    payload.got_value      = 0;

    return(payload);
}

internal_function
void test_ft_lstclear(Tester *tester)
{
    TestParameters tests[] =
    {
        { .size = {__LINE__,  0} }, // The NULL trap (Empty list)
        { .size = {__LINE__,  1} }, // Single node
        { .size = {__LINE__,  3} }, // Standard list
        { .size = {__LINE__, 10} }, // Stress test
    };
    U64 test_count = CountOfStaticArray(tests);

    TemporaryArena temporary_arena = temporary_arena_begin(tester->permanent_arena);

    TestGroup test_group =
    {
        .name                      = String8Literal("ft_lstclear"),
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

        .libft_function           = (void *)ft_lstclear,
        .callback                 = (TestCallbackFunction)callback_for_lstclear,
    };

    run_tests(tester, &test_context);

    if(test_context.flags & TestContextFlag_TestsWereSkipped)
    {
        temporary_arena_end(temporary_arena);
    }
}
