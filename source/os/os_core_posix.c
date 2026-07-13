/*
* Portions of this file are derived from RADDebugger (https://github.com/EpicGamesExt/raddebugger),
* Copyright (c) Epic Games Tools.
* Licensed under the MIT license.
*/
////////////////////////////////
// Memory Allocation

//basic

internal void *os_reserve_memory(U64 size)
{
    void *result = mmap(0, size, PROT_NONE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);
    if(result == MAP_FAILED)
    {
        result = 0;
    }
    return result;
}

internal U32 os_commit_memory(void *memory, U64 size)
{
    mprotect(memory, size, PROT_READ|PROT_WRITE);
    return 1;
}

internal void os_decommit_memory(void *memory, U64 size)
{
    madvise(memory,  size, MADV_DONTNEED);
    mprotect(memory, size, PROT_NONE);
}

internal void os_release_memory(void *memory, U64 size)
{
    munmap(memory, size);
}

// large pages
internal void *os_reserve_large(U64 size)
{
    void *result;
#if OS_LINUX
    result = mmap(0, size, PROT_NONE, MAP_PRIVATE|MAP_ANONYMOUS|MAP_HUGETLB, -1, 0);
#elif OS_MAC
    result = mmap(0, size, PROT_NONE, MAP_PRIVATE|MAP_ANON, -1, 0);
#endif

    if(result == MAP_FAILED)
    {
        result = 0;
    }
    return result;
}

internal U32 os_commit_large(void *memory, U64 size)
{
    mprotect(memory, size, PROT_READ|PROT_WRITE);
    return 1;
}

////////////////////////////////
// Aborting

internal void os_abort(S32 exit_code)
{
    exit(exit_code);
}

internal U64 os_get_timestamp(void)
{
    struct timeval val;
    gettimeofday(&val, 0);
    return (OS_TIMER_FREQUENCY * (U64)val.tv_sec) + (U64)val.tv_usec;
}

internal void os_write_message(String8 message)
{
    write(STDERR_FILENO, message.str, message.size);
}
