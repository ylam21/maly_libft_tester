#undef malloc
#undef free

// Tester Memory Tracking Wrapper Functions
void *__wrap_malloc(size_t size)
{
    void *ptr = malloc(size);
    if(ptr != NULL)
    {
        memset(ptr, 0xCC, size);
        thread_static_allocation_count += 1;
    }
    return(ptr);
}

void __wrap_free(void *ptr)
{
    if(ptr != NULL)
    {
        thread_static_allocation_count -= 1;
        free(ptr);
    }
}

#define malloc  __wrap_malloc
#define free    __wrap_free
