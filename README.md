# The Maly Libft Tester

Currently up-to-date with Libft subject version 19.2.

This tester executes over 500 tests in under 0.2 seconds.<br>

![Tester UI](assets/tester_ui.png)

# Prerequisites
*   Linux
*   GCC compiler
*   Your compiled libft.a library

# Building and running the Tester
Use the provided `build.sh` script.<br>
Pass the path to your compiled `libft.a` archive as the only argument.

```bash
./build.sh <path/to/libft.a>
```
Run the tests:
```bash
./tester
```
For more information (like output files, no-fork mode), run:
```bash
./tester --help
```
# What this tester DOES check?
*   Checks correct outputs for all Libft functions.<br>
*   Checks memory leaks using linker-level wrapping.
*   Checks segmentation faults, timeouts, bus errors, double frees using process isolation.<br>
*   Provides detailed hex and string memory dumps in failed_reports.txt for any failed tests.<br>

# What this tester does NOT check?
*   Does NOT check your if your libft project was written in accorance with the 42 Norm.<br>
*   Does NOT check your libft project's Makefile and its rules, flags, relinking etc.<br>
*   Does NOT check if you declared any global variables in your project. <br>
*   Does NOT check if you used any unallowed functions in your project. <br>
*   Does NOT check for any README requirements.<br>

## Acknowledgments
The base and OS layers of this project are derived from [RADDebugger](https://github.com/EpicGamesExt/raddebugger) by Epic Games. I would like to thank the RAD team for sharing their high-quality code, which significantly improved the robustness and portability of this tester.
