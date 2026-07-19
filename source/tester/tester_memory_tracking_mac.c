#include <stdlib.h>
#include <string.h>

extern thread_static S64 thread_static_allocation_count;

// Native Mach-O DYLD Interposing macro
#define DYLD_INTERPOSE(_replacement,_replacee) \
   __attribute__((used)) static struct{ const void* replacement; const void* replacee; } _interpose_##_replacee \
            __attribute__ ((section ("__DATA,__interpose"))) = { (const void*)(unsigned long)&_replacement, (const void*)(unsigned long)&_replacee };

void *__wrap_malloc(size_t size)
{
    void *ptr = malloc(size);
    if(ptr != NULL)
    {
        memset(ptr, 0xCC, size);
        thread_static_allocation_count += 1;
    }
    return ptr;
}

void __wrap_free(void *ptr)
{
    if(ptr != NULL)
    {
        thread_static_allocation_count -= 1;
        free(ptr);
    }
}

// Register the hooks
DYLD_INTERPOSE(__wrap_malloc, malloc)
DYLD_INTERPOSE(__wrap_free, free)
