internal_function
TestPayload callback_for_lstnew(TestParameters test_parameters)
{
    TestPayload payload = {0};
    TestParameters_Ptr parameters = test_parameters.ptr;

    char *expected_content = (char *)parameters.ptr;
    if (expected_content != 0)
    {
        U64 expected_len = length_of_cstring(expected_content) + 1;
        MemoryCopy(payload.buffer_for_expected, expected_content, expected_len);
        payload.expected_strings_sizes[0]    = expected_len;
        payload.expected_strings_sizes_count = 1;
    }

    global_allocation_count = 0;

    // Call libft function.
    t_list *got_node = ft_lstnew(parameters.ptr);

    if (got_node != 0)
    {
        void *got_content = got_node->content;
        void *got_next    = got_node->next;

        if (got_content != 0)
        {
            U64 got_len = length_of_cstring((char *)got_content) + 1;
            MemoryCopy(payload.buffer_for_got, got_content, got_len);
            payload.got_strings_sizes[0]    = got_len;
            payload.got_strings_sizes_count = 1;
        }

        if ((got_content == parameters.ptr) && (got_next == 0))
        {
            payload.flags |= TestPayloadFlag_ResultsMatch;
        }

        free(got_node);
    }
    else if (expected_content == 0)
    {
        payload.flags |= TestPayloadFlag_ResultsMatch;
    }

    payload.leak_count = global_allocation_count;

    return(payload);
}

internal_function
void test_ft_lstnew(Tester *tester)
{
    TestParameters tests[] =
    {
        { .ptr = {__LINE__, "Hello World"} },
        { .ptr = {__LINE__, "42 Prague"} },
        { .ptr = {__LINE__, ""} },
        { .ptr = {__LINE__, 0} },
    };
    U64 test_count = CountOfStaticArray(tests);

    // Save the current position in Tester's permanent arena and reset it only if tests were skipped.
    TemporaryArena temporary_arena = temporary_arena_begin(tester->permanent_arena);

    TestGroup test_group =
    {
        .name                      = String8Literal("ft_lstnew"),
        .file                      = string8_from_cstring(__FILE__),
        .failed_test_reports       = push_array(tester->permanent_arena, TestReport, test_count),
    };
    TestContext test_context =
    {
        .test_group     = test_group,

        .function_return_type     = TestReturnType_Ptr,
        .function_parameters_type = TestParametersType_Ptr,

        .tests          = tests,
        .test_count     = test_count,

        .libft_function = (void *)ft_lstnew,
        .callback       = (TestCallbackFunction)callback_for_lstnew,
    };

    run_tests(tester, &test_context);

    if(test_context.flags & TestContextFlag_TestsWereSkipped)
    {
        temporary_arena_end(temporary_arena);
    }
}
