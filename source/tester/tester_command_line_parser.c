internal_function
String8 get_tester_version(Arena *arena)
{
    String8 result = push_string8_format(arena, String8Literal("Version %S\n"
                                                             "This tester version is up-to-date with Libft subject version - %S\n"
                                                            ), global_tester_version,
                                                               global_tester_supported_libft_subject_version);
    return(result);
}


read_only global String8 global_tester_help_text = String8Literal(
    "The Maly Libft Tester - Help\n"
    "The following options may be used when starting the tester from the command line:\n\n"
    "  --set-timeout-ms <base-10 value>\n"
    "    Set a timeout for testing in milliseconds.\n"
    "    If a test does not finish under the set timeout, then the test is going to be evaluated as failed.\n"
    "    There is a max bound value of 5000 (5 seconds) that can be set.\n"
    "    The defualt tester's timeout is set to 100 milliseconds (0.1 seconds).\n\n"
    "  --no-colors\n"
    "    Run the tester and print text to the terminal window with no colors.\n\n"
    "  --version\n"
    "    Print the version of the tester with supported libft subject version and exit.\n"
);

internal_function
String8 get_tester_help(void)
{
    return(global_tester_help_text);
}

internal_function
void parse_command_line_to_tester(Tester *tester, int argument_count, char **arguments)
{
    String8 message;
    TemporaryArena scratch = ScratchArenaBegin(0);

    for(int argument_index = 1; argument_index < argument_count; argument_index += 1)
    {
        String8 argument = string8_from_cstring(arguments[argument_index]);

        if(string8_match(argument, String8Literal("--set-timeout-ms")))
        {
            if((argument_index + 1) < argument_count)
            {
                argument_index += 1;
                argument = string8_from_cstring(arguments[argument_index]);
                if(string8_has_base10_digits_only(argument))
                {
                    U64 value = u64_from_string8(argument, 10);
                    if(value >= Thousand(5))
                    {
                        Print("You set 5 or more seconds for as timeout.\n");
                        exit(1);
                    }
                    U64 total_microseconds = value * Thousand(1);
                    U64 seconds = total_microseconds / Million(1);
                    U64 leftover_microseconds = total_microseconds - (seconds * Million(1));

                    struct itimerval timeout = {0};
                    timeout.it_value.tv_sec  = seconds;
                    timeout.it_value.tv_usec = leftover_microseconds;
                    tester->timeout = timeout;
                }
                else
                {
                    Print("Timeout flag (--set-timeout-ms) is followed by a value with non-base 10 digits.\n");
                    exit(1);
                }
            }
            else
            {
                Print("Timeout flag (--set-timeout-ms) is not followed by any argument.\n");
                exit(1);
            }
        }
        else if(string8_match(argument, String8Literal("--no-colors")))
        {
            tester->flags |= TesterFlag_NoColors;
        }
        else if(string8_match(argument, String8Literal("--version")))
        {
            Print("%S\n", get_tester_version(scratch.arena));
            exit(0);
        }
        else if(string8_match(argument, String8Literal("--help")) || string8_match(argument, String8Literal("help")))
        {
            Print("%S\n", get_tester_help());
            exit(0);
        }
        else
        {
            Print("Unrecognized command line argument: %S\n"
                  "For more information run\n"
                  "  %s --help\n\n", argument, arguments[0]);
            exit(0);
        }
    }
    ScratchArenaEnd(scratch);
}
