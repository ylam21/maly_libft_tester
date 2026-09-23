#include "base/base_include.h"
#include "os/os_include.h"
#include "third_party/third_party_include.h"
#include "tester/tester_include.h"

#include "base/base_include.c"
#include "os/os_include.c"
#include "third_party/third_party_include.c"
#include "tester/tester_include.c"

int main(int argument_count, char **arguments)
{
    Tester tester = {0};
    ProfilerReport profiler_report = {0};

    profiler_start();            // Necessary setup for profiling code blocks.
    initialize_os_system_info(); // Necessary setup for creating any new arena.
    initialize_thread_context(); // Necessary setup for the usage of thread local scratch arenas.
    initialize_tester(&tester);

    ProfilerBlockBegin(tester);
    if(argument_count > 1)
    {
        parse_command_line_to_tester(&tester, argument_count, arguments);
    }
    tester_run(&tester);
    ProfilerBlockEnd(tester);

    profiler_report = profiler_end(tester.permanent_arena);
    print_tester_summary(&tester, profiler_report.total_time_elapsed_in_seconds);

    return(0);
}
