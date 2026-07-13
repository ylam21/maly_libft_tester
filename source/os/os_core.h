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
internal void   initialize_os_system_info(void);

////////////////////////////////
// Memory Allocation

// basic
internal void   *os_reserve_memory(U64 size);
internal U32    os_commit_memory(void *memory, U64 size);
internal void   os_decommit_memory(void *memory, U64 size);
internal void   os_release_memory(void *memory, U64 size);

// large pages
internal void   *os_reserve_large(U64 size);
internal U32    os_commit_large(void *memory, U64 size);

////////////////////////////////
// Aborting
internal void   os_abort(S32 exit_code);

////////////////////////////////
// Globals
global OS_SystemInfo global_os_system_info = {0};


#endif // OS_CORE_H
