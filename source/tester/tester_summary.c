internal void print_tester_summary(Tester *tester, F64 time_taken)
{
    TemporaryArena scratch = ScratchArenaBegin(0);
    String8 summary = { .str  = push_array(scratch.arena, U8, 0) };

    U64 total_tests_count = tester->total_tests_passed + tester->total_tests_failed;

    summary.size += push_string8_format(scratch.arena, String8Literal("\n--- Summary ---\n")).size;
    summary.size += push_string8_format(scratch.arena, String8Literal("%-15S %u / %u functions\n"), String8Literal("Tested"), tester->total_groups_tested, TESTER_MAXIMUM_LIBFT_FUNCTION_COUNT).size;
    summary.size += push_string8_format(scratch.arena, String8Literal("%-15S %u\n"), String8Literal("Total Tests"), total_tests_count).size;

    String8 red_color;
    String8 green_color;
    String8 yellow_color;
    String8 reset;
    if(tester->flags & TesterFlag_DisableColors)
    {
        red_color    = String8Literal("");
        green_color  = String8Literal("");
        yellow_color = String8Literal("");
        reset        = String8Literal("");
    }
    else
    {
        red_color    = String8Literal("\033[31m");
        green_color  = String8Literal("\033[32m");
        yellow_color = String8Literal("\033[33m");
        reset        = String8Literal("\033[0m");
    }

    if(tester->total_tests_passed > 0)
    {
        summary.size += push_string8_format(scratch.arena, String8Literal("%S%-15S%S %u\n"), green_color, String8Literal("Passed"), reset, tester->total_tests_passed).size;
    }
    if(tester->total_tests_failed > 0)
    {
        summary.size += push_string8_format(scratch.arena, String8Literal("%S%-15S%S %u\n"), red_color, String8Literal("Failed"), reset, tester->total_tests_failed).size;
    }
    if(tester->total_tests_leaked > 0)
    {
        summary.size += push_string8_format(scratch.arena, String8Literal("%S%-15S%S %u\n"), yellow_color, String8Literal("Leaked"), reset, tester->total_tests_leaked).size;
    }
    if(tester->total_tests_crashed > 0)
    {
        summary.size += push_string8_format(scratch.arena, String8Literal("%S%-15S%S %u\n"), red_color, String8Literal("Crashed"), reset, tester->total_tests_crashed).size;
    }
    if(tester->total_tests_timedout > 0)
    {
        summary.size += push_string8_format(scratch.arena, String8Literal("%-15S %u\n"), red_color, String8Literal("Timed-out"), reset, tester->total_tests_timedout).size;
    }
    if(tester->total_tests_skipped > 0)
    {
        summary.size += push_string8_format(scratch.arena, String8Literal("%-15S %u\n") , String8Literal("Skipped"), tester->total_tests_skipped).size;
    }
    if(time_taken > 0)
    {
        summary.size += push_string8_format(scratch.arena, String8Literal("%-15S %.3fs\n"), String8Literal("Duration"), time_taken).size;
    }
    summary.size += push_string8_format(scratch.arena, String8Literal("\n")).size;

    if(tester->failed_groups_count > 0)
    {
        char *filename = tester->output_filename;
        int fd = open(filename, O_CREAT | O_TRUNC | O_WRONLY, 0777);
        if(fd != -1)
        {
            U32 write_was_successful = build_debug_information_from_tester_and_write_to_fd(fd, tester);
            if(write_was_successful)
            {
                summary.size += push_string8_format(scratch.arena, String8Literal("See '%s' for more debug information.\n"), filename).size;
            }
            else
            {
                summary.size += push_string8_format(scratch.arena, String8Literal("[Error] Failed to write to %s file.\n"), filename).size;
            }
        }
        else
        {
            summary.size += push_string8_format(scratch.arena, String8Literal("[Error] Failed to open %s file.\n"), filename).size;
        }
    }
    else
    {
        summary.size += push_string8_format(scratch.arena, String8Literal("All tests passed.\n")).size;
    }
    write(STDOUT_FILENO, summary.str, summary.size);
    ScratchArenaEnd(scratch);
}
