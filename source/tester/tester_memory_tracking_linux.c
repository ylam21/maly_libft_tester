// Tester Memory Tracking Wrapper Functions
void *__wrap_malloc(size_t size)
{
    void *ptr = __real_malloc(size);
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
        __real_free(ptr);
    }
}
