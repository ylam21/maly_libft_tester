internal void fake_iter_function(void *content)
{
    int *value = (int *)content;
    if(value != 0)
    {
        *value = 42;
    }
}

internal TestPayload callback_for_lstiter(TestParameters test_parameters)
{
    TestPayload payload = {0};
    int list_size = (int)test_parameters.size.size;

    t_list *head = 0;
    for (int index = 0; index < list_size; index += 1)
    {
        t_list *node = (t_list *)malloc(sizeof(t_list));
        int *value   = (int *)malloc(sizeof(int));
        *value = 0;
        node->content = value;
        node->next    = head;
        head          = node;
    }

    // Call libft function.
    ft_lstiter((t_list *)head, (LstiterFunction)fake_iter_function);

    int all_updated = 1;
    int count = 0;
    t_list *current = head;
    while(current != 0)
    {
        if(*(int *)(current->content) != 42)
        {
            all_updated = 0;
        }
        count  += 1;
        current = current->next;
    }

    char expected_string[64];
    int expected_length = snprintf(expected_string, sizeof(expected_string), "%d nodes updated to 42", list_size);
    MemoryCopy(payload.buffer_for_expected, expected_string, expected_length + 1);
    payload.expected_strings_sizes[0] = expected_length + 1;
    payload.expected_strings_sizes_count = 1;

    char got_string[64];
    int got_length = snprintf(got_string, sizeof(got_string), "%d nodes updated to 42", all_updated ? count : -1);
    MemoryCopy(payload.buffer_for_got, got_string, got_length + 1);
    payload.got_strings_sizes[0]    = got_length + 1;
    payload.got_strings_sizes_count = 1;

    if((all_updated == 1) && (count == list_size))
    {
        payload.flags |= TestPayloadFlag_ResultsMatch;
    }

    current = head;
    while(current != 0)
    {
        t_list *next = current->next;
        free(current->content); // Free our int
        free(current);          // Free the node
        current = next;
    }

    payload.expected_value = 0;
    payload.got_value      = 0;

    return(payload);
}
internal void test_ft_lstiter(Tester *tester)
{
    TestParameters tests[] =
    {
        { .size = {__LINE__, 0} }, // The NULL trap
        { .size = {__LINE__, 1} },
        { .size = {__LINE__, 5} },
    };
    U64 test_count = CountOfStaticArray(tests);

    // Save the current position in Tester's permanent arena and reset it only if tests were skipped.
    TemporaryArena temporary_arena = temporary_arena_begin(tester->permanent_arena);

    TestGroup test_group =
    {
        .name                      = String8Literal("ft_lstiter"),
        .file                      = string8_from_cstring(__FILE__),
        .failed_test_reports       = push_array(tester->permanent_arena, TestReport, test_count),
    };
    TestContext test_context =
    {
        .test_group     = test_group,

        .function_return_type     = TestReturnType_Void,
        .function_parameters_type = TestParametersType_Size,

        .tests          = tests,
        .test_count     = test_count,

        .libft_function = (void *)ft_lstiter,
        .callback       = (TestCallbackFunction)callback_for_lstiter,
    };

    run_tests(tester, &test_context);

    if(test_context.flags & TestContextFlag_TestsWereSkipped)
    {
        temporary_arena_end(temporary_arena);
    }
}
