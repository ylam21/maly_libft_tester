// Tester Functions
internal_function
String8 tester_get_version(void)
{
    return(global_tester_version);
}

internal_function
String8 tester_get_supported_libft_subject_version(void)
{
    return(global_tester_supported_libft_subject_version);
}

internal_function
Tester initialize_tester(void)
{
    Tester tester = {0};

    tester.permanent_arena = arena_create();
    tester.testing_arena   = arena_create();
    tester.flags           = TESTER_DEFAULT_FLAGS;
    tester.output_filename = TESTER_DEFAULT_OUTPUT_FILENAME;

    int dev_null_fd = open("/dev/null", O_WRONLY);
    if(dev_null_fd != -1)
    {
        tester.dev_null_fd = dev_null_fd;
    }
    else
    {
        Print("Failed to open '/dev/null' file.\n");
    }

    return(tester);
}

internal_function
void tester_run(Tester *tester)
{
    Print("\n--- Testing for Libft Subject Version %S ---\n", tester_get_supported_libft_subject_version());
    Print("\n--- Part 1 - Libc Functions ---\n");
    test_ft_isalpha(tester);
    test_ft_isdigit(tester);
    test_ft_isalnum(tester);
    test_ft_isascii(tester);
    test_ft_isprint(tester);
    test_ft_strlen(tester);
    test_ft_memset(tester);
    test_ft_bzero(tester);
    test_ft_memcpy(tester);
    test_ft_memmove(tester);
    test_ft_strlcpy(tester);
    test_ft_strlcat(tester);
    test_ft_toupper(tester);
    test_ft_tolower(tester);
    test_ft_strchr(tester);
    test_ft_strncmp(tester);
    test_ft_memchr(tester);
    test_ft_memcmp(tester);
    test_ft_strnstr(tester);
    test_ft_atoi(tester);
    test_ft_calloc(tester);
    test_ft_strdup(tester);

    Print("\n--- Part 2 - Additional Functions ---\n");
    test_ft_substr(tester);
    test_ft_strjoin(tester);
    test_ft_strtrim(tester);
    test_ft_split(tester);
    test_ft_itoa(tester);
    test_ft_strmapi(tester);
    test_ft_striteri(tester);
    test_ft_putchar_fd(tester);
    test_ft_putstr_fd(tester);
    test_ft_putendl_fd(tester);
    test_ft_putnbr_fd(tester);

    Print("\n--- Part 3 - Linked List Functions ---\n");
    test_ft_lstnew(tester);
    test_ft_lstadd_front(tester);
    test_ft_lstsize(tester);
    test_ft_lstlast(tester);
    test_ft_lstadd_back(tester);
    test_ft_lstdelone(tester);
    test_ft_lstclear(tester);
    test_ft_lstiter(tester);
    test_ft_lstmap(tester);
}
