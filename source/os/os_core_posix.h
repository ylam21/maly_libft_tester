/*
* Portions of this file are derived from RADDebugger (https://github.com/EpicGamesExt/raddebugger),
* Copyright (c) Epic Games Tools.
* Licensed under the MIT license.
*/
#ifndef OS_CORE_POSIX_H
#define OS_CORE_POSIX_H

#if OS_LINUX
    #include <linux/limits.h>
    #include <sys/sysinfo.h>
#elif OS_MAC
    #include <mach/mach.h>
    #include <limits.h>
#endif

#include <dirent.h>
#include <dlfcn.h>
#include <dlfcn.h>
#include <errno.h>
#include <execinfo.h>
#include <fcntl.h>
#include <features.h>
#include <pthread.h>
#include <semaphore.h>
#include <signal.h>
#include <spawn.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/random.h>
#include <sys/sendfile.h>
#include <sys/stat.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

internal U64    os_get_timestamp(void);
internal void   os_write_message(String8 message);

#endif // OS_CORE_POSIX_H
