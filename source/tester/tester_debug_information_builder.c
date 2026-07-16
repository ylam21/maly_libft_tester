internal_function
String8 get_memory_view(Arena *arena, void *memory_block)
{
    String8 result =
    {
        .size = 0,
        .str  = push_array(arena, U8, 0),
    };

    if(memory_block != 0)
    {
        result.size += push_string8_format(arena, String8Literal("0x")).size;
        U8 *casted_memory_block = (U8 *)memory_block;
        U64 maximum_byte_index = TESTER_MAXIMUM_VIEW_BYTES_FOR_PRINT;
        for(U64 byte_index = 0; byte_index < maximum_byte_index; byte_index += 1)
        {
            result.size += push_string8_format(arena, String8Literal("%x"), casted_memory_block[byte_index]).size;
        }
    }
    else
    {
        result = global_null_text;
    }

    return(result);
}

internal_function
String8 get_string_view(Arena *arena, void *memory_block)
{
    String8 result =
    {
        .size = 0,
        .str  = push_array(arena, U8, 0),
    };

    if(memory_block != 0)
    {
        U8 *casted_memory_block = (U8 *)memory_block;
        U64 maximum_byte_index = TESTER_MAXIMUM_VIEW_BYTES_FOR_PRINT;
        for(U64 byte_index = 0; byte_index < maximum_byte_index; byte_index += 1)
        {
            U8 character = casted_memory_block[byte_index];
            if(character != '\0')
            {
                result.size += push_string8_format(arena, String8Literal("%c"), character).size;
            }
            else
            {
                break;
            }
        }
    }
    else
    {
        result = global_null_text;
    }

    return(result);
}

internal_function
U64 push_function_parameters_from_test_group(Arena *arena, TestGroup *test_group, U32 test_index)
{
    TemporaryArena scratch = ScratchArenaBegin(arena);
    U64 size = 0;

    size += push_string8_format(arena, String8Literal("[Index] %u\n"), test_index).size;
    size += push_string8_format(arena, String8Literal("[Function Parameters]:\n")).size;
    String8 pointer_text = String8Literal("(Pointer)");
    String8 size_text   = String8Literal("(Size)");
    String8 int_text   = String8Literal("(Int)");
    String8 char_text   = String8Literal("(Char)");
    String8 memory_view;
    String8 string_view;
    TestParameters test = test_group->tests[test_index];
    switch(test_group->function_parameters_type)
    {
        default:
        {
            size += push_string8_format(arena, String8Literal("(Function Parameters Type Not Covered - please report this incident)\n")).size;
        } break;
        case TestParametersType_Void:
        {
            size += push_string8_format(arena, String8Literal("(Void)\n")).size;
        } break;
        case TestParametersType_Int:
        {
            TestParameters_Int function_parameters = (TestParameters_Int)test.single_int;
            size += push_string8_format(arena, String8Literal("%-10S %i\n"), int_text, function_parameters.a).size;
        } break;
        case TestParametersType_PtrSize:
        {
            TestParameters_PtrSize function_parameters = (TestParameters_PtrSize)test.ptr_size;
            memory_view = get_memory_view(scratch.arena, function_parameters.ptr);
            string_view = get_string_view(scratch.arena, function_parameters.ptr);
            size += push_string8_format(arena, String8Literal("%-10S %p -> (Memory View) %S\n"), pointer_text, function_parameters.ptr, memory_view).size;
            size += push_string8_format(arena, String8Literal("%-10S %p -> (String View) %S\n"), pointer_text, function_parameters.ptr, string_view).size;
            size += push_string8_format(arena, String8Literal("%-10S %u\n"), size_text, function_parameters.size).size;
        } break;
        case TestParametersType_Ptr:
        {
            TestParameters_Ptr function_parameters = (TestParameters_Ptr)test.ptr;
            memory_view = get_memory_view(scratch.arena, function_parameters.ptr);
            string_view = get_string_view(scratch.arena, function_parameters.ptr);
            size += push_string8_format(arena, String8Literal("%-10S %p -> (Memory View) %S\n"), pointer_text, function_parameters.ptr, memory_view).size;
            size += push_string8_format(arena, String8Literal("%-10S %p -> (String View) %S\n"), pointer_text, function_parameters.ptr, string_view).size;
        } break;
        case TestParametersType_PtrSizeSize:
        {
            TestParameters_PtrSizeSize function_parameters = (TestParameters_PtrSizeSize)test.ptr_size_size;
            memory_view = get_memory_view(scratch.arena, function_parameters.ptr);
            string_view = get_string_view(scratch.arena, function_parameters.ptr);
            size += push_string8_format(arena, String8Literal("%-10S %p -> (Memory View) %S\n"), pointer_text, function_parameters.ptr, memory_view).size;
            size += push_string8_format(arena, String8Literal("%-10S %p -> (String View) %S\n"), pointer_text, function_parameters.ptr, string_view).size;
            size += push_string8_format(arena, String8Literal("%-10S(1st) %u\n"), size_text, function_parameters.size1).size;
            size += push_string8_format(arena, String8Literal("%-10S(2nd) %u\n"), size_text, function_parameters.size2).size;
        } break;
        case TestParametersType_PtrInt:
        {
            TestParameters_PtrInt function_parameters = (TestParameters_PtrInt)test.ptr_int;
            memory_view = get_memory_view(scratch.arena, function_parameters.ptr);
            string_view = get_string_view(scratch.arena, function_parameters.ptr);
            size += push_string8_format(arena, String8Literal("%-10S %p -> (Memory View) %S\n"), pointer_text, function_parameters.ptr, memory_view).size;
            size += push_string8_format(arena, String8Literal("%-10S %p -> (String View) %S\n"), pointer_text, function_parameters.ptr, string_view).size;
            size += push_string8_format(arena, String8Literal("%-10S %i\n"), size_text, function_parameters.a).size;
        } break;
        case TestParametersType_PtrIntSize:
        {
            TestParameters_PtrIntSize function_parameters = (TestParameters_PtrIntSize)test.ptr_int_size;
            memory_view = get_memory_view(scratch.arena, function_parameters.ptr);
            string_view = get_string_view(scratch.arena, function_parameters.ptr);
            size += push_string8_format(arena, String8Literal("%-10S %p -> (Memory View) %S\n"), pointer_text, function_parameters.ptr, memory_view).size;
            size += push_string8_format(arena, String8Literal("%-10S %p -> (String View) %S\n"), pointer_text, function_parameters.ptr, string_view).size;
            size += push_string8_format(arena, String8Literal("%-10S %u\n"), int_text,  function_parameters.a).size;
            size += push_string8_format(arena, String8Literal("%-10S %u\n"), size_text, function_parameters.size).size;
        } break;
        case TestParametersType_PtrPtrSize:
        {
            TestParameters_PtrPtrSize function_parameters = (TestParameters_PtrPtrSize)test.ptr_ptr_size;
            memory_view = get_memory_view(scratch.arena, function_parameters.ptr1);
            size += push_string8_format(arena, String8Literal("%-10S(1st) %p -> (Memory View) %S\n"), pointer_text, function_parameters.ptr1, memory_view).size;
            memory_view = get_memory_view(scratch.arena, function_parameters.ptr2);
            size += push_string8_format(arena, String8Literal("%-10S(2nd) %p -> (Memory View) %S\n"), pointer_text, function_parameters.ptr2, memory_view).size;
            string_view = get_string_view(scratch.arena, function_parameters.ptr1);
            size += push_string8_format(arena, String8Literal("%-10S(1st) %p -> (String View) %S\n"), pointer_text, function_parameters.ptr1, string_view).size;
            string_view = get_string_view(scratch.arena, function_parameters.ptr2);
            size += push_string8_format(arena, String8Literal("%-10S(2nd) %p -> (String View) %S\n"), pointer_text, function_parameters.ptr2, string_view).size;
            size += push_string8_format(arena, String8Literal("%-10S %u\n"), size_text, function_parameters.size).size;
        } break;
        case TestParametersType_PtrChar:
        {
            TestParameters_PtrChar function_parameters = (TestParameters_PtrChar)test.ptr_char;
            memory_view = get_memory_view(scratch.arena, function_parameters.ptr);
            string_view = get_string_view(scratch.arena, function_parameters.ptr);
            size += push_string8_format(arena, String8Literal("%-10S %p -> (Memory View) %S\n"), pointer_text, function_parameters.ptr, memory_view).size;
            size += push_string8_format(arena, String8Literal("%-10S %p -> (String View) %S\n"), pointer_text, function_parameters.ptr, string_view).size;
            size += push_string8_format(arena, String8Literal("%-10S %c\n"), char_text, function_parameters.c).size;
        } break;
        case TestParametersType_IntPtr:
        {
            TestParameters_IntPtr function_parameters = (TestParameters_IntPtr)test.int_ptr;
            size += push_string8_format(arena, String8Literal("%-10S %i\n"), char_text, function_parameters.a).size;
            memory_view = get_memory_view(scratch.arena, function_parameters.ptr);
            string_view = get_string_view(scratch.arena, function_parameters.ptr);
            size += push_string8_format(arena, String8Literal("%-10S %p -> (Memory View) %S\n"), pointer_text, function_parameters.ptr, memory_view).size;
            size += push_string8_format(arena, String8Literal("%-10S %p -> (String View) %S\n"), pointer_text, function_parameters.ptr, string_view).size;
        } break;
        case TestParametersType_Char:
        {
            TestParameters_Char function_parameters = (TestParameters_Char)test.single_char;
            size += push_string8_format(arena, String8Literal("%-10S %c\n"), char_text, function_parameters.c).size;
        } break;
        case TestParametersType_PtrPtr:
        {
            TestParameters_PtrPtr function_parameters = (TestParameters_PtrPtr)test.ptr_ptr;
            memory_view = get_memory_view(scratch.arena, function_parameters.ptr1);
            size += push_string8_format(arena, String8Literal("%-10S(1st) %p -> (Memory View) %S\n"), pointer_text, function_parameters.ptr1, memory_view).size;
            memory_view = get_memory_view(scratch.arena, function_parameters.ptr2);
            size += push_string8_format(arena, String8Literal("%-10S(2nd) %p -> (Memory View) %S\n"), pointer_text, function_parameters.ptr2, memory_view).size;
            string_view = get_string_view(scratch.arena, function_parameters.ptr1);
            size += push_string8_format(arena, String8Literal("%-10S(1st) %p -> (String View) %S\n"), pointer_text, function_parameters.ptr1, string_view).size;
            string_view = get_string_view(scratch.arena, function_parameters.ptr2);
            size += push_string8_format(arena, String8Literal("%-10S(2nd) %p -> (String View) %S\n"), pointer_text, function_parameters.ptr2, string_view).size;
        } break;
        case TestParametersType_SizeSize:
        {
            TestParameters_SizeSize function_parameters = (TestParameters_SizeSize)test.size_size;
            size += push_string8_format(arena, String8Literal("%-10S(1st) %u\n"), size_text, function_parameters.size1).size;
            size += push_string8_format(arena, String8Literal("%-10S(2nd) %u\n"), size_text, function_parameters.size2).size;
        } break;
        case TestParametersType_Size:
        {
            TestParameters_Size function_parameters = (TestParameters_Size)test.size;
            size += push_string8_format(arena, String8Literal("%-10S %u\n"), size_text, function_parameters.size).size;
        } break;
    }
    size += push_string8_format(arena, String8Literal("\n")).size;
    ScratchArenaEnd(scratch);
    return(size);
}

internal_function
U64 push_results_from_test_payload(Arena *arena, TestGroup *test_group, TestPayload *payload)
{

    U64 size = 0;
    size += push_string8_format(arena, String8Literal("[Return values comparison]:\n")).size;
    String8 expected_return_text = String8Literal("(Expected return value)");
    String8 got_return_text      = String8Literal("(Got return value)     ");
    switch(test_group->function_return_type)
    {
        case TestReturnType_Void:
        {
            size += push_string8_format(arena, String8Literal("%S(Void)\n"), expected_return_text).size;
            size += push_string8_format(arena, String8Literal("%S(Void)\n"), got_return_text).size;
        } break;
        case TestReturnType_Int:
        {
            size += push_string8_format(arena, String8Literal("%S(Int) %i\n"), expected_return_text, payload->expected_value).size;
            size += push_string8_format(arena, String8Literal("%S(Int) %i\n"), got_return_text, payload->got_value).size;
        } break;
        case TestReturnType_Size:
        {
            size += push_string8_format(arena, String8Literal("%S(Size) %u\n"), expected_return_text, payload->expected_value).size;
            size += push_string8_format(arena, String8Literal("%S(Size) %u\n"), got_return_text, payload->got_value).size;
        } break;
        case TestReturnType_Ptr:
        {
            size += push_string8_format(arena, String8Literal("%S(Pointer) %p\n"), expected_return_text, (void *)payload->expected_value).size;
            size += push_string8_format(arena, String8Literal("%S(Pointer) %p\n"), got_return_text, (void *)payload->got_value).size;
        } break;
    }

    if(payload->expected_strings_sizes_count > 0 || payload->got_strings_sizes_count > 0)
    {
        size += push_string8_format(arena, String8Literal("\n[Additional results comparison]:\n")).size;

        size += push_string8_format(arena, String8Literal("(Indexed)(Expected results)(String View):\n")).size;
        U64 buffer_for_expected_offset = 0;
        for(U64 string_index = 0; string_index < payload->expected_strings_sizes_count; string_index += 1)
        {
            U8 *start = payload->buffer_for_expected + buffer_for_expected_offset;
            U64 size  = payload->expected_strings_sizes[string_index];

            size += push_string8_format(arena, String8Literal("(%u) %S\n"), string_index, (String8){ .str = start, .size = size }).size;

            buffer_for_expected_offset += size;
        }

        U64 buffer_for_got_offset = 0;
        size += push_string8_format(arena, String8Literal("(Indexed)(Got results)(String View):\n")).size;
        for(U64 string_index = 0; string_index < payload->got_strings_sizes_count; string_index += 1)
        {
            U8 *start = payload->buffer_for_got + buffer_for_got_offset;
            U64 size  = payload->got_strings_sizes[string_index];

            size += push_string8_format(arena, String8Literal("(%u) %S\n"), string_index, (String8){ .str = start, .size = size }).size;

            buffer_for_got_offset += size;
        }

        size += push_string8_format(arena, String8Literal("\n(Indexed)(Expected results)(Memory View):\n")).size;
        buffer_for_expected_offset = 0;
        for(U64 string_index = 0; string_index < payload->expected_strings_sizes_count; string_index += 1)
        {
            U8 *start = payload->buffer_for_expected + buffer_for_expected_offset;
            U64 size  = payload->expected_strings_sizes[string_index];
            String8 string = (String8){ .str = start, .size = size };

            size += push_string8_format(arena, String8Literal("(%u) "), string_index, string).size;
            for(U64 byte_index = 0; byte_index < string.size; byte_index += 1)
            {
                size += push_string8_format(arena, String8Literal("%x"), string.str[byte_index]).size;
            }
            size += push_string8_format(arena, String8Literal("\n")).size;

            buffer_for_expected_offset += size;
        }

        size += push_string8_format(arena, String8Literal("\n(Indexed)(Expected results)(Memory View):\n")).size;
        buffer_for_got_offset = 0;
        for(U64 string_index = 0; string_index < payload->got_strings_sizes_count; string_index += 1)
        {
            U8 *start = payload->buffer_for_got + buffer_for_got_offset;
            U64 size  = payload->got_strings_sizes[string_index];
            String8 string = (String8){ .str = start, .size = size };

            size += push_string8_format(arena, String8Literal("(%u) "), string_index, string).size;
            for(U64 byte_index = 0; byte_index < string.size; byte_index += 1)
            {
                size += push_string8_format(arena, String8Literal("%x"), string.str[byte_index]).size;
            }
            size += push_string8_format(arena, String8Literal("\n")).size;

            buffer_for_got_offset += size;
        }
    }
    return(size);
}

internal_function
String8 debug_info_from_payload(DebugInfoBuilder *info)
{
    String8 result = { .str  = push_array(info->arena, U8, 0) };

    if(info->payload->flags & TestPayloadFlag_TestPassed)
    {
        info->worker->local_tests_passed += 1;
        *(info->char_to_print) = '.';
    }
    else
    {
        info->worker->local_tests_failed += 1;
        *(info->char_to_print) = 'F';

        if(*(info->header_was_not_copied))
        {
            result.size += push_string8_format(info->arena, String8Literal("----------------------------------------\n")).size;
            result.size += push_string8_format(info->arena, String8Literal("(Start of debug info for failed test group %S)\n"), info->group->name).size;
            result.size += push_string8_format(info->arena, String8Literal("See '%S' file for tests definitions for this group.\n"), info->group->file).size;
            result.size += push_string8_format(info->arena, String8Literal("See test index attached to know where to seek for more information about the test.\n\n")).size;
            *(info->header_was_not_copied) = 0;
        }

        result.size += push_string8_format(info->arena, String8Literal(">>>(Start of test report)>>>\n")).size;
        result.size += push_function_parameters_from_test_group(info->arena, info->group, info->test_index);

        if(!(info->payload->flags & TestPayloadFlag_ResultsMatch))
        {
            result.size += push_string8_format(info->arena, String8Literal("Note: Function did >>NOT<< return correct results.\n\n")).size;
            result.size += push_results_from_test_payload(info->arena, info->group, info->payload);
        }
        else
        {
            result.size += push_string8_format(info->arena, String8Literal("Note: Function did return correct results.\n\n")).size;
        }

        if(info->payload->crash_signal != 0)
        {
            if(info->payload->crash_signal == SIGALRM)
            {
                info->worker->local_tests_timedout += 1;
                *(info->char_to_print) = 'T';
                result.size += push_string8_format(info->arena, String8Literal("%S\n"), global_test_report_error_message_table[TestReportFlag_ErrorTimeout]).size;
            }
            else
            {
                info->worker->local_tests_crashed += 1;
                *(info->char_to_print) = 'C';

                if(info->payload->crash_signal == SIGSEGV)
                {
                    result.size += push_string8_format(info->arena, String8Literal("[Error] Test crashed: Segmentation Fault (Signal 11)\n")).size;
                }
                else if (info->payload->crash_signal == SIGABRT)
                {
                    result.size += push_string8_format(info->arena, String8Literal("[Error] Test crashed: Double Free or Heap Corruption (Signal 6)\n")).size;
                }
                else if(info->payload->crash_signal == SIGBUS)
                {
                    result.size += push_string8_format(info->arena, String8Literal("[Error] Test crashed: Bus Error (Signal 10)\n")).size;
                }
                else
                {
                    result.size += push_string8_format(info->arena, String8Literal("[Error] Test crashed with signal: %u\n"), info->payload->crash_signal).size;
                }
            }
        }
        else if(!(info->payload->flags & TestPayloadFlag_NoMemoryLeak))
        {
            info->worker->local_tests_leaked += 1;
            *(info->char_to_print) = 'M';
            S64 leak_count = info->payload->leak_count;
            String8 object_text = leak_count > 1 ? String8Literal("objects were") : String8Literal("object was");
            result.size += push_string8_format(info->arena, String8Literal("%S %i %S not free'd by %S\n"), global_test_report_error_message_table[TestReportFlag_MemoryLeaked], leak_count, object_text, info->group->name).size;
        }

        result.size += push_string8_format(info->arena, String8Literal("<<<(End of test report)<<<\n\n")).size;
    }
    return(result);
}
