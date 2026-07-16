#ifndef TESTER_TEST_H
#define TESTER_TEST_H

#include <unistd.h> // pipe, read, close
#include <ctype.h>  // islpha, isdigit, isalnum, isascii, isprint, toupper, tolower

// Note from Libft subject (Version 19.2):
//     Some functions that you must reimplement, such as strlcpy, strlcat,
//     and bzero, are not included by default in the GNU C Library (glibc).
//     To test them against the system standard, you may need to include
//     <bsd/string.h> and compile with the -lbsd flag.
//     This behaviour is specific to glibc systems. If you are curious,
//     take the opportunity to explore the differences between glibc and BSD libc.
#include <bsd/string.h>

// Why manual declarations and #pragma weak?
//
// 1. Manual Declarations: We manually declare all libft prototypes and the
//    t_list struct instead of including the student's "libft.h". This isolates
//    the tester from the student's code. If the student has syntax errors,
//    forbidden macros, or missing typedefs in their header file, it will not
//    break the compilation of this tester.
//
// 2. #pragma weak: This tells the linker that these symbols are optional.
//    If a student did not complete the bonus part (or missed a mandatory
//    function), the linker will normally throw a "undefined reference" fatal
//    error and refuse to compile. By declaring them as weak symbols, the
//    linker will silently assign them a memory address of 0 (NULL) if they
//    are missing. The tester can then safely check `if (ft_isalpha != 0)`
//    before attempting to execute tests for ft_isalpha.

// Part 1 - Libc Functions
int					ft_isalpha(int c);
int					ft_isdigit(int c);
int					ft_isalnum(int c);
int					ft_isascii(int c);
int					ft_isprint(int c);
size_t				ft_strlen(const char *s);
void				*ft_memset(void *s, int c, size_t n);
void				ft_bzero(void *s, size_t n);
void				*ft_memcpy(void *dest, const void *src, size_t n);
void				*ft_memmove(void *dest, const void *src, size_t n);
size_t				ft_strlcpy(char *dst, const char *src, size_t size);
size_t				ft_strlcat(char *dst, const char *src, size_t size);
int					ft_toupper(int c);
int					ft_tolower(int c);
char				*ft_strchr(const char *s, int c);
int					ft_strncmp(const char *s1, const char *s2, size_t n);
void				*ft_memchr(const void *s, int c, size_t n);
int					ft_memcmp(const void *s1, const void *s2, size_t n);
char				*ft_strnstr(const char *haystack, const char *needle, size_t n);
int					ft_atoi(const char *s);
void				*ft_calloc(size_t num, size_t size);
char				*ft_strdup(const char *s);

// Part 2 - Additional functions
char				*ft_substr(char const *s, unsigned int start, size_t len);
char				*ft_strjoin(char const *s1, char const *s2);
char				*ft_strtrim(char const *s1, char const *set);
char				**ft_split(char const *s, char c);
char				*ft_itoa(int num);
char				*ft_strmapi(char const *s, char (*f)(unsigned int, char));
void				ft_striteri(char *s, void (*f)(unsigned int, char *));
void				ft_putchar_fd(char c, int fd);
void				ft_putstr_fd(char *s, int fd);
void				ft_putendl_fd(char *s, int fd);
void				ft_putnbr_fd(int n, int fd);

// Part 3 - Linked List
typedef struct s_list
{
    void          *content;
    struct s_list *next;
} t_list;

t_list				*ft_lstnew(void *content);
void				ft_lstadd_front(t_list **lst, t_list *newlst);
int					ft_lstsize(t_list *lst);
t_list				*ft_lstlast(t_list *lst);
void				ft_lstadd_back(t_list **lst, t_list *newlst);
void				ft_lstdelone(t_list *lst, void (*del)(void *));
void				ft_lstclear(t_list **lst, void (*del)(void *));
void				ft_lstiter(t_list *lst, void (*f)(void *));
t_list				*ft_lstmap(t_list *lst, void *(*f)(void *), void (*del)(void *));

// Part 1 - Libc Functions
#pragma weak ft_isalpha       /* 1 */
#pragma weak ft_isdigit       /* 2 */
#pragma weak ft_isalnum       /* 3 */
#pragma weak ft_isascii       /* 4 */
#pragma weak ft_isprint       /* 5 */
#pragma weak ft_strlen        /* 6 */
#pragma weak ft_memset        /* 7 */
#pragma weak ft_bzero         /* 8 */
#pragma weak ft_memcpy        /* 9 */
#pragma weak ft_memmove       /* 10 */
#pragma weak ft_strlcpy       /* 11 */
#pragma weak ft_strlcat       /* 12 */
#pragma weak ft_toupper       /* 13 */
#pragma weak ft_tolower       /* 14 */
#pragma weak ft_strchr        /* 15 */
#pragma weak ft_strncmp       /* 16 */
#pragma weak ft_memchr        /* 17 */
#pragma weak ft_memcmp        /* 18 */
#pragma weak ft_strnstr       /* 19 */
#pragma weak ft_atoi          /* 20 */
#pragma weak ft_calloc        /* 21 */
#pragma weak ft_strdup        /* 22 */

// Part 2 - Additional functions
#pragma weak ft_substr        /* 23 */
#pragma weak ft_strjoin       /* 24 */
#pragma weak ft_strtrim       /* 25 */
#pragma weak ft_split         /* 26 */
#pragma weak ft_itoa          /* 27 */
#pragma weak ft_strmapi       /* 28 */
#pragma weak ft_striteri      /* 29 */
#pragma weak ft_putchar_fd    /* 30 */
#pragma weak ft_putstr_fd     /* 31 */
#pragma weak ft_putendl_fd    /* 32 */
#pragma weak ft_putnbr_fd     /* 33 */

// Part 3 - Linked List
#pragma weak ft_lstnew        /* 34 */
#pragma weak ft_lstadd_front  /* 35 */
#pragma weak ft_lstsize       /* 36 */
#pragma weak ft_lstlast       /* 37 */
#pragma weak ft_lstadd_back   /* 38 */
#pragma weak ft_lstdelone     /* 39 */
#pragma weak ft_lstclear      /* 40 */
#pragma weak ft_lstiter       /* 41 */
#pragma weak ft_lstmap        /* 42 */

// Tester Test Types
#define TESTER_MAXIMUM_PAYLOAD_STRINGS      16
#define TESTER_MAXIMUM_PAYLOAD_BUFFER_SIZE  512

typedef U8 TestPayloadFlags;
enum
{
    TestPayloadFlag_ResultsMatch = (1<<0),
    TestPayloadFlag_NoMemoryLeak = (1<<1),
    TestPayloadFlag_TestPassed   = (1<<2),
};

struct TestPayload
{
    int                 crash_signal;
    TestPayloadFlags    flags;
    S64                 leak_count;

    U64                 expected_value;
    U64                 got_value;

    U64                 expected_strings_sizes[TESTER_MAXIMUM_PAYLOAD_STRINGS];
    U64                 expected_strings_sizes_count;
    U64                 got_strings_sizes[TESTER_MAXIMUM_PAYLOAD_STRINGS];
    U64                 got_strings_sizes_count;

    U8                  buffer_for_expected[TESTER_MAXIMUM_PAYLOAD_BUFFER_SIZE];
    U8                  buffer_for_got[TESTER_MAXIMUM_PAYLOAD_BUFFER_SIZE];
};

// Tester Main Test Functions
internal_function void    *worker_thread_routine(void *params);
internal_function void    run_all_tests_for_test_group_and_evaluate(TestWorkerContext *test_worker, TestGroup *test_group);
internal_function String8 padding_for_stats(Arena *arena, U32 test_count);
internal_function void    crash_handler(int signum);

// Test Callback Functions
internal_function TestPayload callback_for_bzero(TestParameters test_parameters);
internal_function TestPayload callback_for_memchr(TestParameters test_parameters);
internal_function TestPayload callback_for_memcmp(TestParameters test_parameters);
internal_function TestPayload callback_for_memmove(TestParameters test_parameters);
internal_function TestPayload callback_for_isalpha(TestParameters test_parameters);
internal_function TestPayload callback_for_isdigit(TestParameters test_parameters);
internal_function TestPayload callback_for_isprint(TestParameters test_parameters);
internal_function TestPayload callback_for_isalnum(TestParameters test_parameters);
internal_function TestPayload callback_for_isascii(TestParameters test_parameters);
internal_function TestPayload callback_for_tolower(TestParameters test_parameters);
internal_function TestPayload callback_for_toupper(TestParameters test_parameters);
internal_function TestPayload callback_for_atoi(TestParameters test_parameters);
internal_function TestPayload callback_for_itoa(TestParameters test_parameters);
internal_function TestPayload callback_for_memset(TestParameters test_parameters);
internal_function TestPayload callback_for_strlen(TestParameters test_parameters);
internal_function TestPayload callback_for_split(TestParameters test_parameters);
internal_function TestPayload callback_for_strdup(TestParameters test_parameters);
internal_function TestPayload callback_for_strjoin(TestParameters test_parameters);
internal_function TestPayload callback_for_strtrim(TestParameters test_parameters);
internal_function TestPayload callback_for_memcpy(TestParameters test_parameters);
internal_function TestPayload callback_for_strlcpy(TestParameters test_parameters);
internal_function TestPayload callback_for_strlcat(TestParameters test_parameters);
internal_function TestPayload callback_for_strchr(TestParameters test_parameters);
internal_function TestPayload callback_for_strncmp(TestParameters test_parameters);
internal_function TestPayload callback_for_strnstr(TestParameters test_parameters);
internal_function TestPayload callback_for_calloc(TestParameters test_parameters);
internal_function TestPayload callback_for_substr(TestParameters test_parameters);
internal_function TestPayload callback_for_strmapi(TestParameters test_parameters);
internal_function TestPayload callback_for_striteri(TestParameters test_parameters);
internal_function TestPayload callback_for_putstr_fd(TestParameters test_parameters);
internal_function TestPayload callback_for_putendl_fd(TestParameters test_parameters);
internal_function TestPayload callback_for_putchar_fd(TestParameters test_parameters);
internal_function TestPayload callback_for_putnbr_fd(TestParameters test_parameters);
internal_function TestPayload callback_for_lstnew(TestParameters test_parameters);
internal_function TestPayload callback_for_lstadd_front(TestParameters test_parameters);
internal_function TestPayload callback_for_lstsize(TestParameters test_parameters);
internal_function TestPayload callback_for_lstlast(TestParameters test_parameters);
internal_function TestPayload callback_for_lstadd_back(TestParameters test_parameters);
internal_function TestPayload callback_for_lstdelone(TestParameters test_parameters);
internal_function TestPayload callback_for_lstclear(TestParameters test_parameters);
internal_function TestPayload callback_for_lstiter(TestParameters test_parameters);
internal_function TestPayload callback_for_lstmap(TestParameters test_parameters);

// Helper Functions for ft_strmapi
typedef  char(*StrmapiFunction)(unsigned int, char);
internal_function char upper_odd_characters(unsigned int index, char character);
internal_function char lower_odd_characters(unsigned int index, char character);
internal_function char add_index(unsigned int index, char character);
internal_function char return_index_digit(unsigned int index, char character);
internal_function char always_x(unsigned int index, char character);

// Helper Functions for ft_striteri
typedef  void(*StriteriFunction)(unsigned int, char *);
internal_function void make_string_uppercase(unsigned int index, char *character);
internal_function void replace_with_x(unsigned int index, char *character);
internal_function void add_index_to_char(unsigned int index, char *character);

// Helper Functions for ft_lstdelone
typedef  void(*LstdeloneFunction)(void *);
internal_function void fake_delete_function(void *content);

// Helper Functions for ft_lstiter
typedef  void(*LstiterFunction)(void *content);
internal_function void fake_iter_function(void *content);

// Helper Functions for ft_lstmap
internal_function void *fake_map_function(void *content);
internal_function void fake_map_delete_function(void *content);

#endif // TESTER_TESTS_H
