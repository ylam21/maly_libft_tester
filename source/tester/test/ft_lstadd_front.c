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

    // Reset global_allocation_count before calling libft function.
    global_allocation_count = 0;

    // Call libft function.
    ft_lstadd_front((t_list **)&head, (t_list *)new_node);

    payload.leak_count = global_allocation_count;

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

    payload.expected_value = 0;
    payload.got_value      = 0;

    return(payload);
}

internal_function
void test_ft_lstadd_front(Tester *tester)
{
    TestParameters tests[] =
    {
        // ptr1: Content of the existing list (0 means empty list)
        // ptr2: Content of the new node being added to the front

        { .ptr_ptr = {__LINE__, "Old Head", "New Head"} },
        { .ptr_ptr = {__LINE__, "World",    "Hello"} },
        { .ptr_ptr = {__LINE__, 0,          "First Node"} },
        { .ptr_ptr = {__LINE__, "Old Head", ""} },
        { .ptr_ptr = {__LINE__, "",         "New Head"} },
    };
    U64 test_count = CountOfStaticArray(tests);

    // Save the current position in Tester's permanent arena and reset it only if tests were skipped.
    TemporaryArena temporary_arena = temporary_arena_begin(tester->permanent_arena);

    TestGroup test_group =
    {
        .name                      = String8Literal("ft_lstadd_front"),
        .file                      = string8_from_cstring(__FILE__),
        .failed_test_reports       = push_array(tester->permanent_arena, TestReport, test_count),
    };
    TestContext test_context =
    {
        .test_group     = test_group,

        .function_return_type     = TestReturnType_Void,
        .function_parameters_type = TestParametersType_PtrPtr,

        .tests          = tests,
        .test_count     = test_count,

        .libft_function = (void *)ft_lstadd_front,
        .callback       = (TestCallbackFunction)callback_for_lstadd_front,
    };

    run_tests(tester, &test_context);

    if(test_context.flags & TestContextFlag_TestsWereSkipped)
    {
        temporary_arena_end(temporary_arena);
    }
}
