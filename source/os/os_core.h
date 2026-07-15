/*
 * Portions of this file are derived from RADDebugger (https://github.com/EpicGamesExt/raddebugger),
 * Copyright (c) Epic Games Tools.
 * Licensed under the MIT license.
 */
#ifndef OS_CORE_H
#define OS_CORE_H

////////////////////////////////
// System Info
typedef struct OS_SystemInfo OS_SystemInfo;
struct OS_SystemInfo
{
    U64 page_size;
    U64 large_page_size;
};

////////////////////////////////
// System Info
internal_function void   initialize_os_system_info(void);

////////////////////////////////
// Memory Allocation

// basic
internal_function void   *os_reserve_memory(U64 size);
internal_function U32    os_commit_memory(void *memory, U64 size);
internal_function void   os_decommit_memory(void *memory, U64 size);
internal_function void   os_release_memory(void *memory, U64 size);

// large pages
internal_function void   *os_reserve_large(U64 size);
internal_function U32    os_commit_large(void *memory, U64 size);

////////////////////////////////
// Aborting
internal_function void   os_abort(S32 exit_code);

////////////////////////////////
// Globals
global OS_SystemInfo global_os_system_info = {0};


#endif // OS_CORE_H
