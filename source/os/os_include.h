#ifndef OS_INCLUDE_H
#define OS_INCLUDE_H

#include "os_core.h"

#if OS_POSIX
    #include "os_core_posix.h"
#else
    #error OS core layer not implemented for this operating system.
#endif

#endif // OS_INCLUDE_H
