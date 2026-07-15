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
    // Reset global_allocation_count to 1 before calling libft function.
    // Libft function should free the head
    global_allocation_count = 1;

    // Call libft function which should free also this node.
    ft_lstdelone((t_list *)head, (LstdeloneFunction)fake_delete_function);

    if((fake_delete_call_count == 1) && (node_2 != 0))
    {
        payload.flags |= TestPayloadFlag_ResultsMatch;
    }

    payload.leak_count     = global_allocation_count;
    payload.expected_value = 1;
    payload.got_value      = fake_delete_call_count;

    free(node_2);

    return(payload);
}

internal_function
void test_ft_lstdelone(Tester *tester)
{
    TestParameters tests[] =
    {
        { .empty = {__LINE__} },
    };
    U64 test_count = CountOfStaticArray(tests);

     // Save the current position in Tester's permanent arena and reset it only if tests were skipped.
     TemporaryArena temporary_arena = temporary_arena_begin(tester->permanent_arena);

     TestGroup test_group =
     {
         .name                      = String8Literal("ft_lstdelone"),
         .file                      = string8_from_cstring(__FILE__),
         .failed_test_reports       = push_array(tester->permanent_arena, TestReport, test_count),
     };
     TestContext test_context =
     {
         .test_group     = test_group,

         .function_return_type     = TestReturnType_Size,
         .function_parameters_type = TestParametersType_Void,

         .tests          = tests,
         .test_count     = test_count,

         .libft_function = (void *)ft_lstdelone,
         .callback       = (TestCallbackFunction)callback_for_lstdelone,
     };

     run_tests(tester, &test_context);

     if(test_context.flags & TestContextFlag_TestsWereSkipped)
     {
         temporary_arena_end(temporary_arena);
     }
}
