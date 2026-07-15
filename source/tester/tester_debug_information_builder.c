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
U64 push_function_parameters_from_test_report(Arena *arena, TestReport *test_report)
{
    TemporaryArena scratch = ScratchArenaBegin(arena);
    U64 size = 0;

    TestLineNumber test_line_number = test_report->function_parameters.single_int.test_line_number; // do not know if this works
    size += push_string8_format(arena, String8Literal("[Line] %u\n"), test_line_number).size;
    size += push_string8_format(arena, String8Literal("[Function Parameters]:\n")).size;
    String8 pointer_text = String8Literal("(Pointer)");
    String8 size_text   = String8Literal("(Size)");
    String8 int_text   = String8Literal("(Int)");
    String8 char_text   = String8Literal("(Char)");
    String8 memory_view;
    String8 string_view;
    switch(test_report->function_parameters_type)
    {
        // TODO: After writing every test make sure every Type is covered here
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
            TestParameters_Int function_parameters = (TestParameters_Int)test_report->function_parameters.single_int;
            size += push_string8_format(arena, String8Literal("%-10S %i\n"), int_text, function_parameters.a).size;
        } break;
        case TestParametersType_PtrSize:
        {
            TestParameters_PtrSize function_parameters = (TestParameters_PtrSize)test_report->function_parameters.ptr_size;
            memory_view = get_memory_view(scratch.arena, function_parameters.ptr);
            string_view = get_string_view(scratch.arena, function_parameters.ptr);
            size += push_string8_format(arena, String8Literal("%-10S %p -> (Memory View) %S\n"), pointer_text, function_parameters.ptr, memory_view).size;
            size += push_string8_format(arena, String8Literal("%-10S %p -> (String View) %S\n"), pointer_text, function_parameters.ptr, string_view).size;
            size += push_string8_format(arena, String8Literal("%-10S %u\n"), size_text, function_parameters.size).size;
        } break;
        case TestParametersType_Ptr:
        {
            TestParameters_Ptr function_parameters = (TestParameters_Ptr)test_report->function_parameters.ptr;
            memory_view = get_memory_view(scratch.arena, function_parameters.ptr);
            string_view = get_string_view(scratch.arena, function_parameters.ptr);
            size += push_string8_format(arena, String8Literal("%-10S %p -> (Memory View) %S\n"), pointer_text, function_parameters.ptr, memory_view).size;
            size += push_string8_format(arena, String8Literal("%-10S %p -> (String View) %S\n"), pointer_text, function_parameters.ptr, string_view).size;
        } break;
        case TestParametersType_PtrSizeSize:
        {
            TestParameters_PtrSizeSize function_parameters = (TestParameters_PtrSizeSize)test_report->function_parameters.ptr_size_size;
            memory_view = get_memory_view(scratch.arena, function_parameters.ptr);
            string_view = get_string_view(scratch.arena, function_parameters.ptr);
            size += push_string8_format(arena, String8Literal("%-10S %p -> (Memory View) %S\n"), pointer_text, function_parameters.ptr, memory_view).size;
            size += push_string8_format(arena, String8Literal("%-10S %p -> (String View) %S\n"), pointer_text, function_parameters.ptr, string_view).size;
            size += push_string8_format(arena, String8Literal("%-10S(1st) %u\n"), size_text, function_parameters.size1).size;
            size += push_string8_format(arena, String8Literal("%-10S(2nd) %u\n"), size_text, function_parameters.size2).size;
        } break;
        case TestParametersType_PtrInt:
        {
            TestParameters_PtrInt function_parameters = (TestParameters_PtrInt)test_report->function_parameters.ptr_int;
            memory_view = get_memory_view(scratch.arena, function_parameters.ptr);
            string_view = get_string_view(scratch.arena, function_parameters.ptr);
            size += push_string8_format(arena, String8Literal("%-10S %p -> (Memory View) %S\n"), pointer_text, function_parameters.ptr, memory_view).size;
            size += push_string8_format(arena, String8Literal("%-10S %p -> (String View) %S\n"), pointer_text, function_parameters.ptr, string_view).size;
            size += push_string8_format(arena, String8Literal("%-10S %i\n"), size_text, function_parameters.a).size;
        } break;
        case TestParametersType_PtrIntSize:
        {
            TestParameters_PtrIntSize function_parameters = (TestParameters_PtrIntSize)test_report->function_parameters.ptr_int_size;
            memory_view = get_memory_view(scratch.arena, function_parameters.ptr);
            string_view = get_string_view(scratch.arena, function_parameters.ptr);
            size += push_string8_format(arena, String8Literal("%-10S %p -> (Memory View) %S\n"), pointer_text, function_parameters.ptr, memory_view).size;
            size += push_string8_format(arena, String8Literal("%-10S %p -> (String View) %S\n"), pointer_text, function_parameters.ptr, string_view).size;
            size += push_string8_format(arena, String8Literal("%-10S %u\n"), int_text,  function_parameters.a).size;
            size += push_string8_format(arena, String8Literal("%-10S %u\n"), size_text, function_parameters.size).size;
        } break;
        case TestParametersType_PtrPtrSize:
        {
            TestParameters_PtrPtrSize function_parameters = (TestParameters_PtrPtrSize)test_report->function_parameters.ptr_ptr_size;
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
            TestParameters_PtrChar function_parameters = (TestParameters_PtrChar)test_report->function_parameters.ptr_char;
            memory_view = get_memory_view(scratch.arena, function_parameters.ptr);
            string_view = get_string_view(scratch.arena, function_parameters.ptr);
            size += push_string8_format(arena, String8Literal("%-10S %p -> (Memory View) %S\n"), pointer_text, function_parameters.ptr, memory_view).size;
            size += push_string8_format(arena, String8Literal("%-10S %p -> (String View) %S\n"), pointer_text, function_parameters.ptr, string_view).size;
            size += push_string8_format(arena, String8Literal("%-10S %c\n"), char_text, function_parameters.c).size;
        } break;
        case TestParametersType_IntPtr:
        {
            TestParameters_IntPtr function_parameters = (TestParameters_IntPtr)test_report->function_parameters.int_ptr;
            size += push_string8_format(arena, String8Literal("%-10S %i\n"), char_text, function_parameters.a).size;
            memory_view = get_memory_view(scratch.arena, function_parameters.ptr);
            string_view = get_string_view(scratch.arena, function_parameters.ptr);
            size += push_string8_format(arena, String8Literal("%-10S %p -> (Memory View) %S\n"), pointer_text, function_parameters.ptr, memory_view).size;
            size += push_string8_format(arena, String8Literal("%-10S %p -> (String View) %S\n"), pointer_text, function_parameters.ptr, string_view).size;
        } break;
        case TestParametersType_Char:
        {
            TestParameters_Char function_parameters = (TestParameters_Char)test_report->function_parameters.single_char;
            size += push_string8_format(arena, String8Literal("%-10S %c\n"), char_text, function_parameters.c).size;
        } break;
        case TestParametersType_PtrPtr:
        {
            TestParameters_PtrPtr function_parameters = (TestParameters_PtrPtr)test_report->function_parameters.ptr_ptr;
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
            TestParameters_SizeSize function_parameters = (TestParameters_SizeSize)test_report->function_parameters.size_size;
            size += push_string8_format(arena, String8Literal("%-10S(1st) %u\n"), size_text, function_parameters.size1).size;
            size += push_string8_format(arena, String8Literal("%-10S(2nd) %u\n"), size_text, function_parameters.size2).size;
        } break;
        case TestParametersType_Size:
        {
            TestParameters_Size function_parameters = (TestParameters_Size)test_report->function_parameters.size;
            size += push_string8_format(arena, String8Literal("%-10S %u\n"), size_text, function_parameters.size).size;
        } break;
    }
    size += push_string8_format(arena, String8Literal("\n")).size;
    ScratchArenaEnd(scratch);
    return(size);
}

internal_function
U64 push_results_from_test_report(Arena *arena, TestReport *test_report)
{

    U64 size = 0;
    size += push_string8_format(arena, String8Literal("[Return values comparison]:\n")).size;
    String8 expected_return_text = String8Literal("(Expected return value)");
    String8 got_return_text      = String8Literal("(Got return value)     ");
    switch(test_report->function_return_type)
    {
        case TestReturnType_Void:
        {
            size += push_string8_format(arena, String8Literal("%S(Void)\n"), expected_return_text).size;
            size += push_string8_format(arena, String8Literal("%S(Void)\n"), got_return_text).size;
        } break;
        case TestReturnType_Int:
        {
            size += push_string8_format(arena, String8Literal("%S(Int) %i\n"), expected_return_text, test_report->expected_value).size;
            size += push_string8_format(arena, String8Literal("%S(Int) %i\n"), got_return_text, test_report->got_value).size;
        } break;
        case TestReturnType_Size:
        {
            size += push_string8_format(arena, String8Literal("%S(Size) %u\n"), expected_return_text, test_report->expected_value).size;
            size += push_string8_format(arena, String8Literal("%S(Size) %u\n"), got_return_text, test_report->got_value).size;
        } break;
        case TestReturnType_Ptr:
        {
            size += push_string8_format(arena, String8Literal("%S(Pointer) %p\n"), expected_return_text, (void *)test_report->expected_value).size;
            size += push_string8_format(arena, String8Literal("%S(Pointer) %p\n"), got_return_text, (void *)test_report->got_value).size;
        } break;
    }

    if(test_report->expected_strings_count > 0 || test_report->got_strings_count > 0)
    {
        size += push_string8_format(arena, String8Literal("\n[Additional results comparison]:\n")).size;

        size += push_string8_format(arena, String8Literal("(Indexed)(Expected results)(String View):\n")).size;
        for(U64 string_index = 0; string_index < test_report->expected_strings_count; string_index += 1)
        {
            String8 string = test_report->expected_strings[string_index];
            size += push_string8_format(arena, String8Literal("(%u) %S\n"), string_index, string).size;
        }

        size += push_string8_format(arena, String8Literal("(Indexed)(Got results)(String View):\n")).size;
        for(U64 string_index = 0; string_index < test_report->got_strings_count; string_index += 1)
        {
            String8 string = test_report->got_strings[string_index];
            size += push_string8_format(arena, String8Literal("(%u) %S\n"), string_index, string).size;
        }

        size += push_string8_format(arena, String8Literal("\n(Indexed)(Expected results)(Memory View):\n")).size;
        for(U64 string_index = 0; string_index < test_report->expected_strings_count; string_index += 1)
        {
            String8 string = test_report->expected_strings[string_index];
            size += push_string8_format(arena, String8Literal("(%u) "), string_index, string).size;
            for(U64 byte_index = 0; byte_index < string.size; byte_index += 1)
            {
                size += push_string8_format(arena, String8Literal("%x"), string.str[byte_index]).size;
            }
            size += push_string8_format(arena, String8Literal("\n")).size;
        }

        size += push_string8_format(arena, String8Literal("(Indexed)(Got results)(Memory View):\n")).size;
        for(U64 string_index = 0; string_index < test_report->got_strings_count; string_index += 1)
        {
            String8 string = test_report->got_strings[string_index];
            size += push_string8_format(arena, String8Literal("(%u) "), string_index, string).size;
            for(U64 byte_index = 0; byte_index < string.size; byte_index += 1)
            {
                size += push_string8_format(arena, String8Literal("%x"), string.str[byte_index]).size;
            }
            size += push_string8_format(arena, String8Literal("\n")).size;
        }
    }
    return(size);
}

internal_function
U64 push_general_error_messages_from_test_report(Arena *arena, TestReport *test_report, TestGroup *test_group)
{
    U64 size = 0;
    if(test_report->flags & TestReportFlag_ErrorPipe)
    {
        size += push_string8_format(arena, String8Literal("%S\n"), global_test_report_error_message_table[TestReportFlag_ErrorPipe]).size;
    }
    if(test_report->flags & TestReportFlag_ErrorPayloadRead)
    {
        size += push_string8_format(arena, String8Literal("%S\n"), global_test_report_error_message_table[TestReportFlag_ErrorPayloadRead]).size;
    }
    if(test_report->flags & TestReportFlag_ErrorFork)
    {
        size += push_string8_format(arena, String8Literal("%S\n"), global_test_report_error_message_table[TestReportFlag_ErrorFork]).size;
    }
    if(test_report->flags & TestReportFlag_ErrorTimeout)
    {
        size += push_string8_format(arena, String8Literal("%S\n"), global_test_report_error_message_table[TestReportFlag_ErrorTimeout]).size;
    }
    if(test_report->flags & TestReportFlag_MemoryLeaked)
    {
        S64 leak_count = test_report->leak_count;
        if(leak_count > 1)
        {
            size += push_string8_format(arena, String8Literal("%S %i objects were not free'd by %S\n"), global_test_report_error_message_table[TestReportFlag_MemoryLeaked], leak_count, test_group->name).size;
        }
        else
        {
            size += push_string8_format(arena, String8Literal("%S %i object was not free'd by %S\n"), global_test_report_error_message_table[TestReportFlag_MemoryLeaked], leak_count, test_group->name).size;
        }
    }
    if(test_report->flags & TestReportFlag_ErrorErrnoSet)
    {
        size += push_string8_format(arena, String8Literal("%S %u\n"), global_test_report_error_message_table[TestReportFlag_ErrorErrnoSet], test_report->error_code).size;
    }
    if(test_report->flags & TestReportFlag_ErrorExitNonZero)
    {
        size += push_string8_format(arena, String8Literal("%S %u\n"), global_test_report_error_message_table[TestReportFlag_ErrorErrnoSet], test_report->error_code).size;
    }
    if(test_report->flags & TestReportFlag_ErrorChildCrashed)
    {
        if (test_report->error_code == SIGSEGV)
        {
            size += push_string8_format(arena, String8Literal("[Error] Crash: Segmentation Fault (Signal 11)\n")).size;
        }
        else if (test_report->error_code == SIGABRT)
        {
            size += push_string8_format(arena, String8Literal("[Error] Crash: Double Free or Heap Corruption (Signal 6)\n")).size;
        }
        else if(test_report->error_code == SIGBUS)
        {
            size += push_string8_format(arena, String8Literal("[Error] Crash: Bus Error (Signal 10)\n")).size;
        }
        else if(test_report->error_code == SIGALRM)
        {
            size += push_string8_format(arena, String8Literal("[Error] Crashed with SIGALRM: Infinite Loop Detected\n")).size;
        }
        else
        {
            size += push_string8_format(arena, String8Literal("[Error] Child process crashed with signal: %u\n"), test_report->error_code).size;
        }
    }
    return(size);
}

internal_function
U64 push_debug_information_from_test_report(Arena *arena, TestReport *test_report, TestGroup *test_group)
{
    U64 size = 0;
    size += push_string8_format(arena, String8Literal(">>>(Start of test report)>>>\n")).size;
    size += push_function_parameters_from_test_report(arena, test_report);
    if(test_report->flags & TestReportFlag_ResultsDoNotMatch)
    {
        size += push_string8_format(arena, String8Literal("Function returned incorrect results.\n\n")).size;
        size += push_results_from_test_report(arena, test_report);
    }
    else
    {
        size += push_string8_format(arena, String8Literal("Function returned correct results.\n\n")).size;
    }
    size += push_general_error_messages_from_test_report(arena, test_report, test_group);
    size += push_string8_format(arena, String8Literal("<<<(End of test report)<<<\n\n")).size;
    return(size);
}


internal_function
U32 build_debug_information_from_tester_and_write_to_fd(int fd, Tester *tester)
{
    TemporaryArena scratch = ScratchArenaBegin(0);
    String8 result =
    {
        .str  = push_array(scratch.arena, U8, 0),
    };

    for(U64 group_index = 0; group_index < tester->failed_groups_count; group_index += 1)
    {
        TestGroup group = tester->failed_groups[group_index];

        result.size += push_string8_format(scratch.arena, String8Literal("----------------------------------------\n")).size;
        result.size += push_string8_format(scratch.arena, String8Literal("(Start of debug information for failed test group %S)\n"), group.name).size;
        result.size += push_string8_format(scratch.arena, String8Literal("Group Name: %S\n"), group.name).size;
        result.size += push_string8_format(scratch.arena, String8Literal("See '%S' file for tests definitions for this group.\n"), group.file).size;
        result.size += push_string8_format(scratch.arena, String8Literal("See file's line number attached to each test for more information.\n\n")).size;
        result.size += push_string8_format(scratch.arena, String8Literal("Total failed tests for this group is %u.\n\n"), group.failed_test_reports_count).size;
        for(U64 test_report_index = 0; test_report_index < group.failed_test_reports_count; test_report_index += 1)
        {
            TestReport test_report = group.failed_test_reports[test_report_index];
            result.size += push_debug_information_from_test_report(scratch.arena, &test_report, &group);
        }
        result.size += push_string8_format(scratch.arena, String8Literal("(End of debug information for failed test group %S)\n"), group.name).size;
        result.size += push_string8_format(scratch.arena, String8Literal("----------------------------------------\n")).size;
    }

    U32 write_was_successful = 0;
    if(write(fd, result.str, result.size) != -1)
    {
        write_was_successful = 1;
    }
    ScratchArenaEnd(scratch);
    return(write_was_successful);
}
