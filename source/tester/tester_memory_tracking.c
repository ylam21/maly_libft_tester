// Tester Memory Tracking Wrapper Functions
void *__wrap_malloc(size_t size)
{
    void *ptr = __real_malloc(size);
    if(ptr != NULL)
    {
        global_allocation_count += 1;
    }
    return(ptr);
}

void __wrap_free(void *ptr)
{
    if(ptr != NULL)
    {
        global_allocation_count -= 1;
        __real_free(ptr);
    }
}

void *__wrap_calloc(size_t nmemb, size_t size)
{
    void *ptr = __real_calloc(nmemb, size);
    if(ptr != NULL)
    {
        global_allocation_count += 1;
    }
    return(ptr);
}

void *__wrap_realloc(void *ptr, size_t size)
{
    void *new_ptr = __real_realloc(ptr, size);

    if(ptr == NULL && new_ptr != NULL)
    {
        global_allocation_count += 1;
    }
    else if(ptr != NULL && size == 0)
    {
        global_allocation_count -= 1;
    }
    return(new_ptr);
}
