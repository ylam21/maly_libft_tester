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

    // Reset thread_static_allocation_count before calling libft function.
    thread_static_allocation_count = 0;

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

    payload.leak_count     = thread_static_allocation_count;
    payload.expected_value = (U64)expected_return;
    payload.got_value      = (U64)got_return;

    return(payload);
}

read_only global TestGroup test_group_ft_split =
{
    .tests =
    {
        [0]  = { .ptr_char_ptr_size = {"Hello World",              ' ',  (char *[]){"Hello", "World"},                2} },
        [1]  = { .ptr_char_ptr_size = {"lorem ipsum dolor sit",    ' ',  (char *[]){"lorem", "ipsum", "dolor", "sit"},4} },
        [2]  = { .ptr_char_ptr_size = {"  Hello  World  ",         ' ',  (char *[]){"Hello", "World"},                2} },
        [3]  = { .ptr_char_ptr_size = {",,,,,a,,,,,,b,,,,,",       ',',  (char *[]){"a", "b"},                        2} },
        [4]  = { .ptr_char_ptr_size = {"---Empty---Edges---",      '-',  (char *[]){"Empty", "Edges"},                2} },
        [5]  = { .ptr_char_ptr_size = {"split|||||this|||||string",'|',  (char *[]){"split", "this", "string"},       3} },
        [6]  = { .ptr_char_ptr_size = {"Hello World",              'X',  (char *[]){"Hello World"},                   1} },
        [7]  = { .ptr_char_ptr_size = {"a",                        'b',  (char *[]){"a"},                             1} },
        [8]  = { .ptr_char_ptr_size = {"",                         ' ',  (char *[]){0},                               0} },
        [9]  = { .ptr_char_ptr_size = {"     ",                    ' ',  (char *[]){0},                               0} },
        [10] = { .ptr_char_ptr_size = {"||||||",                   '|',  (char *[]){0},                               0} },
        [11] = { .ptr_char_ptr_size = {"a",                        'a',  (char *[]){0},                               0} },
        [12] = { .ptr_char_ptr_size = {"This is a string",         '\0', (char *[]){"This is a string"},              1} },
        [13] = { .ptr_char_ptr_size = {"",                         '\0', (char *[]){0},                               0} },
        [14] = { .ptr_char_ptr_size = {"lorem\xffipsum\xff" "dolor",  '\xff',(char *[]){"lorem", "ipsum", "dolor"},   3} },
    },
    .test_count               = 15,
    .name                     = String8Literal("ft_split"),
    .file                     = String8Literal(__FILE__),
    .function_return_type     = TestReturnType_Ptr,
    .function_parameters_type = TestParametersType_PtrChar,
    .libft_function           = (void *)ft_split,
    .callback                 = (TestCallbackFunction)callback_for_split,
};
