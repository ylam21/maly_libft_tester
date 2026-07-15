internal_function
TestPayload callback_for_memset(TestParameters test_parameters)
{
    TestPayload payload = {0};

    TestParameters_PtrIntSize parameters = test_parameters.ptr_int_size;

    // Backup memory to backup buffer.
    U8 backup_buffer[TESTER_MAXIMUM_PAYLOAD_BUFFER_SIZE];
    MemoryCopy(backup_buffer, parameters.ptr, parameters.size);

    // Call libc function and save the result.
    void *expected_return = memset((void *)parameters.ptr, (int)parameters.a, (size_t)parameters.size);
    if(expected_return != 0)
    {
        MemoryCopy(payload.buffer_for_expected, expected_return, parameters.size);
        payload.expected_strings_sizes[0]    = parameters.size;
        payload.expected_strings_sizes_count = 1;
    }

    // Restore memory with backup buffer.
    MemoryCopy(parameters.ptr, backup_buffer, parameters.size);

    // Reset global_allocation_count before calling libft function.
    global_allocation_count = 0;

    // Call libft function and save the result.
    void *got_return = ft_memset((void *)parameters.ptr, (int)parameters.a, (size_t)parameters.size);
    if(got_return != 0)
    {
        MemoryCopy(payload.buffer_for_got, got_return, parameters.size);
        payload.got_strings_sizes[0]    = parameters.size;
        payload.got_strings_sizes_count = 1;
    }

    if((got_return == expected_return) && MemoryMatch(payload.buffer_for_expected, payload.buffer_for_got, parameters.size))
    {
        payload.flags |= TestPayloadFlag_ResultsMatch;
    }

    payload.leak_count     = global_allocation_count;
    payload.expected_value = (U64)expected_return;
    payload.got_value      = (U64)got_return;

    // Restore memory with backup buffer for the next test.
    MemoryCopy(parameters.ptr, backup_buffer, parameters.size);

    return(payload);
}

internal_function
void test_ft_memset(Tester *tester)
{
    U64 memory_block_size = TESTER_MAXIMUM_PAYLOAD_BUFFER_SIZE;
    U8 *memory_block      = push_array(tester->testing_arena, U8, memory_block_size);
    memory_set_u32(memory_block, 0xDEADBEEF, memory_block_size);

    TestParameters tests[] =
    {
        // 1. Basic Standard Fills
        { .ptr_int_size = {__LINE__, memory_block, 'A', 10               } },
        { .ptr_int_size = {__LINE__, memory_block, 0,   32               } },
        { .ptr_int_size = {__LINE__, memory_block, 'X', memory_block_size} },

        // 2. Zero-length (Should do nothing, not segfault)
        { .ptr_int_size = {__LINE__, memory_block, 'A', 0                } },
        { .ptr_int_size = {__LINE__, memory_block + 5, 'B', 0            } }, // Unaligned 0 size

        // 3. The Unsigned Char Cast Traps (CRITICAL)
        // memset treats 'c' as an unsigned char. Students who use (char) instead of (unsigned char) fail these.
        { .ptr_int_size = {__LINE__, memory_block, 256, 10               } }, // 256 % 256 = 0
        { .ptr_int_size = {__LINE__, memory_block, -1,  10               } }, // -1 becomes 255
        { .ptr_int_size = {__LINE__, memory_block, -42, 15               } },
        { .ptr_int_size = {__LINE__, memory_block, 2147483647, 5         } }, // INT_MAX

        // 4. Single byte and Unaligned Sizes
        { .ptr_int_size = {__LINE__, memory_block, 'Z', 1                } },
        { .ptr_int_size = {__LINE__, memory_block, 'Y', 17               } }, // Prime number size

        // 5. Unaligned Pointers (Catches bad word-size optimizations)
        { .ptr_int_size = {__LINE__, memory_block + 1, 'C', 42           } },
        { .ptr_int_size = {__LINE__, memory_block + 3, 'D', 15           } },
        { .ptr_int_size = {__LINE__, memory_block + 7, 'E', 1            } },

        // 6. Unaligned Pointer + Max boundaries
        { .ptr_int_size = {__LINE__, memory_block + 1, 'F', memory_block_size - 1} },
        { .ptr_int_size = {__LINE__, memory_block + 127, 'G', memory_block_size - 127} },
    };
    U64 test_count = CountOfStaticArray(tests);

    // Save the current position in Tester's permanent arena and reset it only if tests were skipped.
    TemporaryArena temporary_arena = temporary_arena_begin(tester->permanent_arena);

    TestGroup test_group =
    {
        .name                      = String8Literal("ft_memset"),
        .file                      = string8_from_cstring(__FILE__),
        .failed_test_reports       = push_array(tester->permanent_arena, TestReport, test_count),
    };
    TestContext test_context =
    {
        .test_group     = test_group,

        .function_return_type     = TestReturnType_Ptr,
        .function_parameters_type = TestParametersType_PtrIntSize,

        .tests          = tests,
        .test_count     = test_count,

        .libft_function = (void *)ft_memset,
        .callback       = (TestCallbackFunction)callback_for_memset,
    };

    run_tests(tester, &test_context);

    if(test_context.flags & TestContextFlag_TestsWereSkipped)
    {
        temporary_arena_end(temporary_arena);
    }
}
