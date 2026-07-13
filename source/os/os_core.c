/*
* Portions of this file are derived from RADDebugger (https://github.com/EpicGamesExt/raddebugger),
* Copyright (c) Epic Games Tools.
* Licensed under the MIT license.
*/
// System Info
internal void initialize_os_system_info(void)
{
    global_os_system_info.page_size       = (U64)getpagesize();
    global_os_system_info.large_page_size = MiB(2);
}
