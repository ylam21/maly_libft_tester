internal_function
TestPayload callback_for_split(TestParameters test_parameters)
{
    TestPayload payload = {0};

    TestParameters_PtrCharPtrSize parameters = test_parameters.ptr_char_ptr_size;

    char **expected_return = (char **)parameters.ptr2;
    if(expected_return != 0)
    {
        U64 buffer_offset = 0;
        for(U64 string_index = 0; string_index < parameters.size; string_index += 1)
        {
            char *string      = expected_return[string_index];
            U64 string_length = length_of_cstring(string) + 1;
            MemoryCopy(payload.buffer_for_expected + buffer_offset, string, string_length);
            payload.expected_strings_sizes[string_index] = string_length;
            payload.expected_strings_sizes_count += 1;
            buffer_offset += string_length;
        }
    }

    // Reset global_allocation_count before calling libft function.
    global_allocation_count = 0;

    // Call libft function and save the result.
    char **got_return = ft_split((const char *)parameters.ptr1, parameters.c);
    if(got_return != 0)
    {
        U64 buffer_offset = 0;
        U64 string_index = 0;
        char *string = got_return[0];
        while(string != 0)
        {
            U64 string_length = length_of_cstring(string) + 1;
            MemoryCopy(payload.buffer_for_got + buffer_offset, string, string_length);
            payload.got_strings_sizes[string_index] = string_length;
            payload.got_strings_sizes_count += 1;
            buffer_offset += string_length;
            free(string);
            string_index += 1;
            string        = got_return[string_index];
        }
        free(string);
        free(got_return);

        if((expected_return != 0) && (string_index == payload.got_strings_sizes_count))
        {
            U64 offset_for_expected_buffer = 0;
            U64 offset_for_got_buffer      = 0;
            U32 every_string_matches = 1;
            for(U64 index = 0; index < payload.expected_strings_sizes_count; index += 1)
            {
                U8 *expected_string      = payload.buffer_for_expected + offset_for_expected_buffer;
                U64 expected_string_size = payload.expected_strings_sizes[index];

                U8 *got_string           = payload.buffer_for_got + offset_for_got_buffer;
                U64 got_string_size      = payload.got_strings_sizes[index];

                if((got_string_size != expected_string_size) || (MemoryMatch(got_string, expected_string, expected_string_size) == 0))
                {
                    every_string_matches = 0;
                    break;
                }

                offset_for_expected_buffer += expected_string_size;
                offset_for_got_buffer      += got_string_size;
            }

            if(every_string_matches)
            {
                payload.flags |= TestPayloadFlag_ResultsMatch;
            }
        }
    }
    else
    {
        if(expected_return == 0) // This should never be the case, but if it is, handle it.
        {
            payload.flags |= TestPayloadFlag_ResultsMatch;
        }
    }

    payload.leak_count     = global_allocation_count;
    payload.expected_value = (U64)expected_return;
    payload.got_value      = (U64)got_return;

    return(payload);
}

internal_function
void test_ft_split(Tester *tester)
{
    TestParameters tests[] =
    {
        // 1. Basic Normal Splits
        { .ptr_char_ptr_size = {__LINE__, "Hello World",              ' ',  (char *[]){"Hello", "World"},                2} },
        { .ptr_char_ptr_size = {__LINE__, "lorem ipsum dolor sit",    ' ',  (char *[]){"lorem", "ipsum", "dolor", "sit"},4} },

        // 2. Delimiter clustering (Start, Middle, End)
        { .ptr_char_ptr_size = {__LINE__, "  Hello  World  ",         ' ',  (char *[]){"Hello", "World"},                2} },
        { .ptr_char_ptr_size = {__LINE__, ",,,,,a,,,,,,b,,,,,",       ',',  (char *[]){"a", "b"},                        2} },
        { .ptr_char_ptr_size = {__LINE__, "---Empty---Edges---",      '-',  (char *[]){"Empty", "Edges"},                2} },
        { .ptr_char_ptr_size = {__LINE__, "split|||||this|||||string",'|',  (char *[]){"split", "this", "string"},       3} },

        // 3. No delimiters found
        { .ptr_char_ptr_size = {__LINE__, "Hello World",              'X',  (char *[]){"Hello World"},                   1} },
        { .ptr_char_ptr_size = {__LINE__, "a",                        'b',  (char *[]){"a"},                             1} },

        // 4. Edge Cases: Empty strings and strings of ONLY delimiters (Size MUST be 0)
        { .ptr_char_ptr_size = {__LINE__, "",                         ' ',  (char *[]){0},                               0} },
        { .ptr_char_ptr_size = {__LINE__, "     ",                    ' ',  (char *[]){0},                               0} },
        { .ptr_char_ptr_size = {__LINE__, "||||||",                   '|',  (char *[]){0},                               0} },
        { .ptr_char_ptr_size = {__LINE__, "a",                        'a',  (char *[]){0},                               0} },

        // 5. Splitting by the Null Terminator
        // Since \0 is the end of the string, it should return the whole string as one word
        { .ptr_char_ptr_size = {__LINE__, "This is a string",         '\0', (char *[]){"This is a string"},              1} },
        { .ptr_char_ptr_size = {__LINE__, "",                         '\0', (char *[]){0},                               0} },

        // 6. Non-standard ASCII delimiters
        { .ptr_char_ptr_size = {__LINE__, "lorem\xffipsum\xff" "dolor",  '\xff',(char *[]){"lorem", "ipsum", "dolor"},   3} },
    };
    U64 test_count = CountOfStaticArray(tests);

    // Save the current position in Tester's permanent arena and reset it only if tests were skipped.
    TemporaryArena temporary_arena = temporary_arena_begin(tester->permanent_arena);

    TestGroup test_group =
    {
        .name                      = String8Literal("ft_split"),
        .file                      = string8_from_cstring(__FILE__),
        .failed_test_reports       = push_array(tester->permanent_arena, TestReport, test_count),
    };
    TestContext test_context =
    {
        .test_group     = test_group,

        .function_return_type     = TestReturnType_Ptr,
        .function_parameters_type = TestParametersType_PtrChar,

        .tests          = tests,
        .test_count     = test_count,

        .libft_function = (void *)ft_split,
        .callback       = (TestCallbackFunction)callback_for_split,
    };

    run_tests(tester, &test_context);

    if(test_context.flags & TestContextFlag_TestsWereSkipped)
    {
        temporary_arena_end(temporary_arena);
    }
}
