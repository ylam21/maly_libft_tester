#include "os_core.c"

#if OS_POSIX
    #include "os_core_posix.c"
#else
    #error OS core layer not implemented for this operating system.
#endif
