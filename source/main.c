#ifndef BUILD_DEBUG
    #define BUILD_DEBUG        0
#endif // BUILD_DEBUG

#ifndef ENABLE_PROFILER
    #define ENABLE_PROFILER    1
#endif // ENABLE_PROFILER

#include "base/base_include.h"
#include "os/os_include.h"
#include "tester/tester_include.h"

#include "base/base_include.c"
#include "os/os_include.c"
#include "tester/tester_include.c"

int main(int argument_count, char **arguments)
{
    profiler_start();            // Necessary setup for profiling code block.
    initialize_os_system_info(); // Necessary setup for creating any new arena.
    initialize_thread_context(); // Necessary setup for the usage of scratch arenas.

    ProfilerBlockBegin(tester);

    Tester tester = initialize_tester();

    if(argument_count > 1)
    {
        parse_command_line_to_tester(&tester, arguments, (U64)argument_count);
    }

    tester_run(&tester);

    ProfilerBlockEnd(tester);

    ProfilerReport profiler_report = profiler_end(tester.permanent_arena);

    print_tester_summary(&tester, profiler_report.total_time_elapsed_in_seconds);
    write(STDOUT_FILENO, profiler_report.full_report.str, profiler_report.full_report.size);

    return(0);
}
