internal_function
String8 get_tester_version(Arena *arena)
{
    String8 result = push_string8_format(arena, String8Literal("Version %S\n"
                                                             "This tester version is up-to-date with Libft subject version - %S\n"
                                                            ), tester_get_version(),
                                                               tester_get_supported_libft_subject_version());
    return(result);
}

internal_function
String8 get_tester_help(void)
{
    String8 result = String8Literal(
        "The Maly Libft Tester - Help\n"
        "The following options may be used when starting the tester from the command line:\n\n"
        "  --output <filename>\n"
        "    Run the tester with provided filename.\n"
        "    If no output file is set from command line the tester proceeds with its default output filename.\n"
        "    If any test failes then debug information will be written to this file.\n"
        "    If all tests pass then no debug information will be written and output file will not be opened.\n\n"
        "  --no-colors\n"
        "    Run the tester and print text to the terminal window with no colors.\n\n"
        "  --version\n"
        "    Print the version of the tester with supported libft subject version and exit.\n"
    );

    return(result);
}

internal_function
void parse_command_line_to_tester(Tester *tester, char **arguments, U64 argument_count)
{
    String8 message;
    TemporaryArena scratch = ScratchArenaBegin(0);

    for(U64 argument_index = 1; argument_index < argument_count; argument_index += 1)
    {
        String8 argument = string8_from_cstring(arguments[argument_index]);

        if(string8_match(argument, String8Literal("--output")))
        {
            if((argument_index + 1) < argument_count)
            {
                tester->output_filename = arguments[argument_index + 1];
                argument_index += 1;
            }
            else
            {
                String8 message = push_string8_format(scratch.arena, String8Literal("Output flag (--output) is not followed by filename.\n\n"
                                                                                    ));
                write(STDOUT_FILENO, message.str, message.size);
                exit(0);
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
            message = push_string8_format(scratch.arena, String8Literal("Unrecognized command line argument: %S\n"
                                                                        "For more information run\n"
                                                                        "  %s --help\n\n"
                                                                        ), argument, arguments[0]);
            write(STDOUT_FILENO, message.str, message.size);
            exit(0);
        }
    }
    ScratchArenaEnd(scratch);
}
