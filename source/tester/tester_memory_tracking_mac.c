#include <stdlib.h>
#include <string.h>
#include <malloc/malloc.h>

void *__wrap_malloc(size_t size) {
    return malloc(size);
}
void __wrap_free(void *ptr) {
    free(ptr);
}

void *malloc(size_t size)
{
    void *ptr = malloc_zone_malloc(malloc_default_zone(), size);
    if (ptr != NULL)
    {
        memset(ptr, 0xCC, size);
        thread_static_allocation_count += 1;
    }
    return ptr;
}

void free(void *ptr)
{
    if (ptr != NULL)
    {
        thread_static_allocation_count -= 1;
        malloc_zone_free(malloc_default_zone(), ptr);
    }
}
