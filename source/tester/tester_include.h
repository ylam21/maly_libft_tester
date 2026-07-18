#ifndef TESTER_INCLUDE_H
#define TESTER_INCLUDE_H

#include "tester.h"
#include "tester_command_line_parser.h"
#include "tester_summary.h"
#if   OS_LINUX
    #include "tester_memory_tracking_linux.h"
#elif OS_MAC
    #include "tester_memory_tracking_mac.h"
#endif
#include "tester_debug_information_builder.h"
#include "test/tester_test.h"

#endif // TESTER_INCLUDE_H
