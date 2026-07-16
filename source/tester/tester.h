#ifndef TESTER_H
#define TESTER_H

// Note on test group sizes:
// Avoid creating a test group with exactly 3 tests. There is a known formatting
// quirk in certain terminal emulators where exactly three passing dots ("...")
// are rendered with irregular padding, breaking the visual alignment of the output.

// Tester Default Settings
#define TESTER_DEFAULT_FLAGS                    0
#define TESTER_DEFAULT_OUTPUT_FILENAME          "failed_reports.txt"
#define TESTER_MAXIMUM_TEST_GROUP_COUNT         42 // 42 was counted in source/tester/tester.c
#define TESTER_MAXIMUM_TESTS_FOR_GROUP_COUNT    26 // Currently, max tests written is 26 tests for ft_memmove

// Tester Types
typedef U8 TestReturnType;
enum
{
    TestReturnType_Void = (1<<0),
    TestReturnType_Int  = (1<<1),
    TestReturnType_Size = (1<<2), // Note: I use the word 'Size' for unsigned integer
    TestReturnType_Ptr  = (1<<3),
};

// Test Parameters Types
typedef struct TestParameters_Int TestParameters_Int;
struct TestParameters_Int
{
    S64               a;
};

typedef struct TestParameters_Ptr TestParameters_Ptr;
struct TestParameters_Ptr
{
    void               *ptr;
};


typedef struct TestParameters_IntPtr TestParameters_IntPtr;
struct TestParameters_IntPtr
{
    S64                a;
    void               *ptr;
};

typedef struct TestParameters_PtrPtrSize TestParameters_PtrPtrSize;
struct TestParameters_PtrPtrSize
{
    void                *ptr1;
    void                *ptr2;
    U64                 size;
};

typedef struct TestParameters_PtrSize TestParameters_PtrSize;
struct TestParameters_PtrSize
{
    void                *ptr;
    U64                 size;
};

typedef struct TestParameters_PtrIntSize TestParameters_PtrIntSize;
struct TestParameters_PtrIntSize
{
    void                *ptr;
    S64                 a;
    U64                 size;
};

typedef struct TestParameters_PtrChar TestParameters_PtrChar;
struct TestParameters_PtrChar
{
    void                *ptr;
    S64                 c;
};

typedef struct TestParameters_PtrCharPtrSize TestParameters_PtrCharPtrSize;
struct TestParameters_PtrCharPtrSize
{
    void                *ptr1;
    S64                 c;
    void                *ptr2;
    U64                 size;
};

typedef struct TestParameters_PtrPtrPtr TestParameters_PtrPtrPtr;
struct TestParameters_PtrPtrPtr
{
    void                *ptr1;
    void                *ptr2;
    void                *ptr3;
};

typedef struct TestParameters_PtrPtr TestParameters_PtrPtr;
struct TestParameters_PtrPtr
{
    void                *ptr1;
    void                *ptr2;
};

typedef struct TestParameters_PtrInt TestParameters_PtrInt;
struct TestParameters_PtrInt
{
    void                *ptr;
    S64                 a;
};

typedef struct TestParameters_SizeSize TestParameters_SizeSize;
struct TestParameters_SizeSize
{
    U64                 size1;
    U64                 size2;
};

typedef struct TestParameters_Size TestParameters_Size;
struct TestParameters_Size
{
    U64                 size;
};

typedef struct TestParameters_PtrSizeSize TestParameters_PtrSizeSize;
struct TestParameters_PtrSizeSize
{
    void                *ptr;
    U64                 size1;
    U64                 size2;
};

typedef struct TestParameters_PtrSizeSizePtr TestParameters_PtrSizeSizePtr;
struct TestParameters_PtrSizeSizePtr
{
    void                *ptr1;
    U64                 size1;
    U64                 size2;
    void                *ptr2;
};

typedef struct TestParameters_CharPtr TestParameters_CharPtr;
struct TestParameters_CharPtr
{
    char                c;
    void                *ptr;
};

typedef struct TestParameters_Char TestParameters_Char;
struct TestParameters_Char
{
    char                c;
};

typedef union TestParameters TestParameters;
union TestParameters
{
    TestParameters_Int                single_int;        // Note: used for - isalpha
    TestParameters_Char               single_char;       // Note: used for - ft_putchar_fd
    TestParameters_IntPtr             int_ptr;           // Note: used for - itoa
    TestParameters_Ptr                ptr;               // Note: used for - atoi
    TestParameters_PtrSize            ptr_size;          // Note: used for - bzero
    TestParameters_PtrInt             ptr_int;           // Note: used for - strchr
    TestParameters_PtrIntSize         ptr_int_size;      // Note: used for - memchr
    TestParameters_PtrPtrSize         ptr_ptr_size;      // Note: used for - memcmp
    TestParameters_PtrChar            ptr_char;          // Note: used for - ft_split
    TestParameters_PtrCharPtrSize     ptr_char_ptr_size; // Note: used for - ft_split
    TestParameters_PtrPtr             ptr_ptr;           // Note: used for - ft_strjoin
    TestParameters_PtrPtrPtr          ptr_ptr_ptr;       // Note: used for - ft_strjoin
    TestParameters_SizeSize           size_size;         // Note: used for - ft_calloc
    TestParameters_Size               size;              // Note: used for - lstsize
    TestParameters_PtrSizeSize        ptr_size_size;     // Note: used for - substr
    TestParameters_PtrSizeSizePtr     ptr_size_size_ptr; // Note: used for - substr
    TestParameters_CharPtr            char_ptr;          // Note: used for - ft_putchar_fd
};

typedef U32 TestParametersType;
enum
{
    TestParametersType_Void           = (1<<0),
    TestParametersType_Int            = (1<<1),
    TestParametersType_Size           = (1<<2),
    TestParametersType_Ptr            = (1<<3),
    TestParametersType_PtrIntSize     = (1<<4),
    TestParametersType_PtrPtrSize     = (1<<5),
    TestParametersType_PtrSize        = (1<<6),
    TestParametersType_IntPtr         = (1<<7),
    TestParametersType_PtrChar        = (1<<8),
    TestParametersType_PtrCharPtrSize = (1<<9),
    TestParametersType_PtrPtrPtr      = (1<<10),
    TestParametersType_PtrPtr         = (1<<11),
    TestParametersType_PtrInt         = (1<<12),
    TestParametersType_SizeSize       = (1<<13),
    TestParametersType_PtrSizeSize    = (1<<14),
    TestParametersType_PtrSizeSizePtr = (1<<15),
    TestParametersType_CharPtr        = (1<<16),
    TestParametersType_Char           = (1<<17),
};


typedef U16 TestReportFlags;
enum
{
    TestReportFlag_TestPassed         = (1<<0),
    TestReportFlag_MemoryLeaked       = (1<<1),
    TestReportFlag_ResultsDoNotMatch  = (1<<2),
    TestReportFlag_ErrorPipe          = (1<<3),
    TestReportFlag_ErrorFork          = (1<<4),
    TestReportFlag_ErrorErrnoSet      = (1<<5),
    TestReportFlag_ErrorExitNonZero   = (1<<6),
    TestReportFlag_ErrorChildCrashed  = (1<<7),
    TestReportFlag_ErrorPayloadRead   = (1<<8),
    TestReportFlag_ErrorTimeout       = (1<<9),
};


typedef struct TestGroup TestGroup;
typedef struct TestReport TestReport;
typedef struct TestWorkerContext TestWorkerContext;
typedef struct Tester Tester;
typedef struct TestPayload TestPayload;

typedef TestPayload (*TestCallbackFunction)(TestParameters);

typedef U8 TesterFlags;
enum
{
    TesterFlag_NoColors = (1<<0),
};

struct TestReport
{
    TestReportFlags     flags;
    S64                 error_code;
    TestGroup           *test_group;
    U32                 test_index;
    TestPayload         *test_payload;
};

struct TestWorkerContext
{
    TesterFlags flags;

    U32         test_group_start_index;
    U32         test_group_end_index;

    U32         local_test_groups_tested;
    U32         local_tests_passed;
    U32         local_tests_failed;
    U32         local_tests_leaked;
    U32         local_tests_crashed;
    U32         local_tests_timedout;
    U32         local_tests_skipped;

    String8     local_test_groups_summary;
    String8     local_test_groups_report;
};

struct TestGroup
{
    TestParameters       tests[TESTER_MAXIMUM_TESTS_FOR_GROUP_COUNT];
    U32                  test_count;
    String8              name;
    String8              file;
    TestReturnType       function_return_type;
    TestParametersType   function_parameters_type;
    void                 *libft_function;
    TestCallbackFunction callback;
};



struct Tester
{
    Arena       *permanent_arena;

    String8     report;

    U32         total_test_groups_tested;
    U64         total_tests_passed;
    U64         total_tests_failed;
    U64         total_tests_leaked;
    U64         total_tests_crashed;
    U64         total_tests_timedout;
    U64         total_tests_skipped;

    char        *output_filename;
    int         dev_null_fd;

    TesterFlags flags;
};

// Globals
global String8 global_tester_version                         = String8Literal("1.0.3");
global String8 global_tester_supported_libft_subject_version = String8Literal("19.2");

global String8 global_symbol_missing_text = String8Literal("symbol missing in libft.a");
global String8 global_null_text = String8Literal("(null)");
global String8 global_color_table_for_characters[]  =
{
    ['.'] = String8Literal("\033[32m.\033[0m"),
    ['M'] = String8Literal("\033[33mM\033[0m"),
    ['C'] = String8Literal("\033[31mC\033[0m"),
    ['T'] = String8Literal("\033[31mT\033[0m"),
    ['F'] = String8Literal("\033[31mF\033[0m"),
};
global String8 global_test_report_error_message_table[] =
{
    [TestReportFlag_ErrorPipe]        = String8Literal("[Error] Call to pipe() failed"),
    [TestReportFlag_ErrorPayloadRead] = String8Literal("[Error] Payload Read Failed"),
    [TestReportFlag_ErrorFork]        = String8Literal("[Error] Call to fork() failed"),
    [TestReportFlag_MemoryLeaked]     = String8Literal("[Error] Memory Leak Detected:"),
    [TestReportFlag_ErrorErrnoSet]    = String8Literal("[Error] waitpid() failed and errno was set to:"),
    [TestReportFlag_ErrorExitNonZero] = String8Literal("[Error] Child process exited with non-zero value:"),
    [TestReportFlag_ErrorTimeout]     = String8Literal("[Error] Child process timed-out after 1 second."),
};
global int global_dev_null_fd = -1;

// Tester Functions
internal_function String8    tester_get_version(void);
internal_function String8    tester_get_supported_libft_subject_version(void);
internal_function void       tester_run(Tester *tester);
internal_function Tester     initialize_tester(void);

#endif // TESTER_H
