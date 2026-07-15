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

    // Reset global_allocation_count before calling libft function.
    global_allocation_count = 0;

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

    payload.leak_count     = global_allocation_count;
    payload.expected_value = (U64)expected_size;
    payload.got_value      = (U64)got_size;

    return(payload);
}

internal_function
void test_ft_lstsize(Tester *tester)
{
    TestParameters tests[] =
    {
        // The NULL trap
        { .size = {__LINE__, 0} },

        // Standard sizes
        { .size = {__LINE__, 1} },
        { .size = {__LINE__, 2} },
        { .size = {__LINE__, 5} },

        // Stress test
        { .size = {__LINE__, 42} },
    };
    U64 test_count = CountOfStaticArray(tests);

    // Save the current position in Tester's permanent arena and reset it only if tests were skipped.
    TemporaryArena temporary_arena = temporary_arena_begin(tester->permanent_arena);

    TestGroup test_group =
    {
        .name                      = String8Literal("ft_lstsize"),
        .file                      = string8_from_cstring(__FILE__),
        .failed_test_reports       = push_array(tester->permanent_arena, TestReport, test_count),
    };
    TestContext test_context =
    {
        .test_group     = test_group,

        .function_return_type     = TestReturnType_Int,
        .function_parameters_type = TestParametersType_Size,

        .tests          = tests,
        .test_count     = test_count,

        .libft_function = (void *)ft_lstsize,
        .callback       = (TestCallbackFunction)callback_for_lstsize,
    };

    run_tests(tester, &test_context);

    if(test_context.flags & TestContextFlag_TestsWereSkipped)
    {
        temporary_arena_end(temporary_arena);
    }
}
