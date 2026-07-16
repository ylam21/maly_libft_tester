internal_function
TestPayload callback_for_putnbr_fd(TestParameters test_parameters)
{
    TestPayload payload = {0};
    TestParameters_IntPtr parameters = test_parameters.int_ptr;

    char *expected_string = (char *)parameters.ptr;
    U64 expected_length = length_of_cstring(expected_string);

    MemoryCopy(payload.buffer_for_expected, expected_string, expected_length + 1);
    payload.expected_strings_sizes[0]    = expected_length + 1;
    payload.expected_strings_sizes_count = 1;

    thread_static_allocation_count = 0;

    int pipe_fd[2];
    if(pipe(pipe_fd) != -1)
    {
        // Call the libft function.
        ft_putnbr_fd((int)parameters.a, pipe_fd[1]);

        close(pipe_fd[1]);

        char got_buffer[TESTER_MAXIMUM_PAYLOAD_BUFFER_SIZE];
        MemoryZero(got_buffer, TESTER_MAXIMUM_PAYLOAD_BUFFER_SIZE);

        int bytes_read = read(pipe_fd[0], got_buffer, TESTER_MAXIMUM_PAYLOAD_BUFFER_SIZE - 1);
        close(pipe_fd[0]);

        if(bytes_read >= 0)
        {
            got_buffer[bytes_read] = '\0';

            U64 got_length = bytes_read;
            MemoryCopy(payload.buffer_for_got, got_buffer, got_length + 1);
            payload.got_strings_sizes[0]    = got_length + 1;
            payload.got_strings_sizes_count = 1;

            if((got_length == expected_length) && MemoryMatch(payload.buffer_for_got, payload.buffer_for_expected, expected_length))
            {
                payload.flags |= TestPayloadFlag_ResultsMatch;
            }
        }
    }

    payload.leak_count = thread_static_allocation_count;

    return(payload);
}

read_only global TestGroup test_group_ft_putnbr_fd =
{
    .tests =
    {
        [0] = { .int_ptr = {0,           "0"} },
        [1] = { .int_ptr = {42,          "42"} },
        [2] = { .int_ptr = {-42,         "-42"} },
        [3] = { .int_ptr = {2147483647,  "2147483647"} },
        [4] = { .int_ptr = {-2147483648, "-2147483648"} },
    },
    .test_count               = 5,
    .name                     = String8Literal("ft_putnbr_fd"),
    .file                     = String8Literal(__FILE__),
    .function_return_type     = TestReturnType_Void,
    .function_parameters_type = TestParametersType_Int,
    .libft_function           = (void *)ft_putnbr_fd,
    .callback                 = (TestCallbackFunction)callback_for_putnbr_fd,
};
