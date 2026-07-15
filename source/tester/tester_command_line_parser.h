#ifndef TESTER_COMMAND_LINE_PARSER_H
#define TESTER_COMMAND_LINE_PARSER_H

internal_function void    parse_command_line_to_tester(Tester *tester, char **arguments, U64 argument_count);
internal_function String8 get_tester_help(void);
internal_function String8 get_tester_version(Arena *arena);

#endif // TESTER_COMMAND_LINE_PARSER_H
