internal_function
void profiler_start(void)
{
	profiler.os_time_start = os_get_timestamp();
	profiler.profiler_tsc_start = __rdtsc();
}

internal_function
ProfilerReport profiler_end(Arena *arena)
{
    ProfilerReport report = {0};

    profiler.os_time_end = os_get_timestamp();
	profiler.profiler_tsc_end = __rdtsc();

	U64 os_time_elapsed = profiler.os_time_end - profiler.os_time_start;
	U64 total_cpu_elapsed = profiler.profiler_tsc_end - profiler.profiler_tsc_start;

	F64 total_time_elapsed_in_seconds    = (F64)os_time_elapsed / (F64)OS_TIMER_FREQUENCY;
	report.total_time_elapsed_in_seconds = total_time_elapsed_in_seconds;

	String8 final_string_start = (String8){0};
	U64 final_string_size = 0;
	if (os_time_elapsed > 0 && total_cpu_elapsed > 0)
	{
		U64 cpu_freq = OS_TIMER_FREQUENCY * total_cpu_elapsed / os_time_elapsed;

		final_string_start = push_string8_format(arena, String8Literal("\n--- PROFILING RESULTS ---\n"));
		final_string_size += final_string_start.size;
		final_string_size += push_string8_format(arena, String8Literal("Total time: %u milliseconds (CPU freq: %u)\n"), os_time_elapsed / 1000, cpu_freq).size;

		#if ENABLE_PROFILER
		for (U64 index = 0; index < CountOfStaticArray(profiler_blocks); index += 1)
		{
			ProfilerBlock *b = &profiler_blocks[index];
			if (b->hit_count > 0)
			{
				F64 inc_percent = ((F64)b->tsc_inclusive / (F64)total_cpu_elapsed) * 100.0;
				F64 exc_percent = ((F64)b->tsc_exclusive / (F64)total_cpu_elapsed) * 100.0;

				F64 gib_per_sec = 0;
				F64 bytes_per_sec; // Delete Later
				if (b->processed_byte_count > 0)
				{
				    F64 seconds = (F64)b->tsc_inclusive / (F64)cpu_freq;
					// F64 bytes_per_sec = b->processed_byte_count / seconds; // Get back later
					bytes_per_sec = b->processed_byte_count / seconds;
					gib_per_sec = (F64)bytes_per_sec / (F64)GiB(1);
				}
				if (inc_percent == exc_percent)
				{
				    // Note: This block does not have any nested/children blocks
					final_string_size +=  push_string8_format(arena, String8Literal("%-20S: %-12u cycles [%5.2f%%] | %-19S | Hits %-8u"),
					b->name,
					b->tsc_inclusive, inc_percent,
					String8Literal("No children blocks"),
					b->hit_count).size;
				}
				else
				{
				    final_string_size += push_string8_format(arena, String8Literal("%-20S: %-12u cycles [%5.2f%%] | Exclusive: [%5.2f%%] | Hits %-8u"),
			        b->name,
				    b->tsc_inclusive, inc_percent,
			        exc_percent,
				    b->hit_count).size;
				}
				if (b->processed_byte_count > 0)
				{
				    F64 mib_count = (F64)b->processed_byte_count / (F64)MiB(1);
					// final_string_size += push_string8_format(arena, String8Literal(" | %5.3f MiB at %5.3f GiB/s\n"), mib_count, gib_per_sec).size;
					(void)gib_per_sec;
					final_string_size += push_string8_format(arena, String8Literal(" | %u tests at %5.3f tests/s\n"), b->processed_byte_count, bytes_per_sec).size;
				}
				else
				{
                    final_string_size += push_string8_format(arena, String8Literal("\n")).size;
				}
			}
		}
		#endif // ENABLE_PROFILER
		final_string_size += push_string8_format(arena, String8Literal("-------------------------\n")).size;
	}

	report.full_report = (String8){.str = final_string_start.str, .size = final_string_size};

	return(report);
}
