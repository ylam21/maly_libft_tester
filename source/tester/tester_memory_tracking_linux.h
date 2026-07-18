#ifndef TESTER_MEMORY_TRACKING_H
#define TESTER_MEMORY_TRACKING_H

extern void *__real_malloc(size_t size);
extern void  __real_free(void *ptr);

// Tester Memory Tracking Wrapper Functions
void *__wrap_malloc(size_t size);
void __wrap_free(void *ptr);

// Thread Local
thread_static S64 thread_static_allocation_count = 0;

#endif // TESTER_MEMORY_TRACKING_H
