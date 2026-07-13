internal void *fake_map_function(void *content)
{
    int *old_value = (int *)content;
    int *new_value = (int *)malloc(sizeof(int));
    if(new_value != 0)
    {
        *new_value = *old_value * 2;
    }
    return((void *)new_value);
}

internal void fake_map_delete_function(void *content)
{
    free(content);
}

internal TestPayload callback_for_lstmap(TestParameters test_parameters)
{
    TestPayload payload = {0};
    int list_size = (int)test_parameters.size.size;

    t_list *old_head = 0;
    for (int index = 0; index < list_size; index += 1)
    {
        t_list *node = (t_list *)malloc(sizeof(t_list));
        int *value   = (int *)malloc(sizeof(int));
        *value = 10;
        node->content = value;
        node->next    = old_head;
        old_head      = node;
    }

    // Reset global_allocation_count before calling libft function.
    global_allocation_count = 0;

    // Call libft function.
    t_list *new_head = ft_lstmap((t_list *)old_head, fake_map_function, fake_map_delete_function);

    int is_perfect = 1;
    int old_count = 0;
    int new_count = 0;

    t_list *curr_old = old_head;
    while(curr_old != 0)
    {
        if(*(int *)(curr_old->content) != 10)
        {
            is_perfect = 0;
        }
        old_count += 1;
        curr_old   = curr_old->next;
    }

    t_list *curr_new = new_head;
    while(curr_new != 0)
    {
        if(*(int *)(curr_new->content) != 20)
        {
            is_perfect = 0;
        }
        new_count += 1;;
        curr_new   = curr_new->next;
    }

    if((is_perfect) && (old_count == list_size) && (new_count == list_size))
    {
        if(list_size == 0 || (global_allocation_count == list_size * 2))
        {
            payload.flags |= TestPayloadFlag_ResultsMatch;
        }
    }

    char expected_string[64];
    snprintf(expected_string, sizeof(expected_string), "Old: %dx10 | New: %dx20", list_size, list_size);
    MemoryCopy(payload.buffer_for_expected, expected_string, length_of_cstring(expected_string) + 1);
    payload.expected_strings_sizes[0]    = length_of_cstring(expected_string) + 1;
    payload.expected_strings_sizes_count = 1;

    char got_string[64];
    snprintf(got_string, sizeof(got_string), "Old: %dx10 | New: %dx20", old_count, new_count);
    MemoryCopy(payload.buffer_for_got, got_string, length_of_cstring(got_string) + 1);
    payload.got_strings_sizes[0]    = length_of_cstring(got_string) + 1;
    payload.got_strings_sizes_count = 1;

    curr_old = old_head;
    while(curr_old != 0)
    {
        t_list *next = curr_old->next;
        free(curr_old->content);
        free(curr_old);
        curr_old = next;
    }

    curr_new = new_head;
    while(curr_new != 0)
    {
        t_list *next = curr_new->next;
        free(curr_new->content);
        free(curr_new);
        curr_new = next;
    }

    // We expect the global allocations to equal list_size * 2, so we override the leak count
    payload.leak_count     = (global_allocation_count == (S64)(list_size * 2)) ? 0 : global_allocation_count;
    payload.expected_value = 0;
    payload.got_value      = 0;

    return(payload);
}

internal void test_ft_lstmap(Tester *tester)
{
    TestParameters tests[] =
    {
        { .size = {__LINE__,  0} }, // The NULL trap (Must return NULL safely)
        { .size = {__LINE__,  1} }, // Single node map
        { .size = {__LINE__,  3} }, // Standard map
        { .size = {__LINE__, 10} }, // Stress test
    };
    U64 test_count = CountOfStaticArray(tests);


    TemporaryArena temporary_arena = temporary_arena_begin(tester->permanent_arena);

    TestGroup test_group =
    {
        .name                      = String8Literal("ft_lstmap"),
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

        .libft_function           = (void *)ft_lstmap,
        .callback                 = (TestCallbackFunction)callback_for_lstmap,
    };

    run_tests(tester, &test_context);

    if(test_context.flags & TestContextFlag_TestsWereSkipped)
    {
        temporary_arena_end(temporary_arena);
    }
}
