#ifndef TESTER_MEMORY_TRACKING_H
#define TESTER_MEMORY_TRACKING_H

extern void *__real_malloc(size_t size);
extern void  __real_free(void *ptr);
extern void *__real_calloc(size_t nmemb, size_t size);
extern void *__real_realloc(void *ptr, size_t size);

// Tester Memory Tracking Wrapper Functions
void *__wrap_malloc(size_t size);
void __wrap_free(void *ptr);
void *__wrap_calloc(size_t nmemb, size_t size);
void *__wrap_realloc(void *ptr, size_t size);

// Thread Local
thread_static S64 thread_static_allocation_count = 0;

#endif // TESTER_MEMORY_TRACKING_H
