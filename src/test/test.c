#include "test/test.h"

#include "core/log.h"

#include <string.h>

bool test_assert_equal_char_buffer(const char* expect, const char* actual)
{
    log_push_indent(LOG_STDOUT | LOG_TEST);

    log_msg(LOG_STDOUT | LOG_TEST, "Test char buffers.");
    log_format_msg(LOG_STDOUT | LOG_TEST, "Expect: %s", expect);
    log_format_msg(LOG_STDOUT | LOG_TEST, "Actual: %s", actual);

    int expect_len = strlen(expect);
    int actual_len = strlen(actual);

    if(expect_len != actual_len)
    {
        log_format_msg(LOG_STDOUT | LOG_TEST, "Expected length %d, actual length %d", expect_len, actual_len);
        goto test_assert_equal_char_buffer_fail;
    }

    for(int i = 0; i < expect_len; ++i)
    {
        if(expect[i] != actual[i])
        {
            log_format_msg(LOG_STDOUT | LOG_TEST, "Buffers differ at index %d", i);
            goto test_assert_equal_char_buffer_fail;
        }
    }

    log_pop_indent(LOG_STDOUT | LOG_TEST);
    return TEST_CASE_SUCCESS;

test_assert_equal_char_buffer_fail:
    log_pop_indent(LOG_STDOUT | LOG_TEST);
    return TEST_CASE_FAIL;
}

bool test_assert_equal_int(const int expect, const int actual)
{
    return expect == actual;
}

bool test_assert_equal_bool(const bool expect, const bool actual)
{
    return expect == actual;
}

bool test_assert_not_null(void* value)
{
    return value != NULL;
}

bool test_assert_null(void* value)
{
    return value == NULL;
}

void test_run_test_block(const char* block_name, test_func func)
{
    log_msg(LOG_STDOUT | LOG_TEST, "");
    log_format_msg(LOG_STDOUT | LOG_TEST, "Test Block: %s", block_name);
    log_push_indent(LOG_STDOUT | LOG_TEST);

    bool success = func();

    log_msg(LOG_STDOUT | LOG_TEST, success ? "SUCCESS" : "FAILED");
    log_pop_indent(LOG_STDOUT | LOG_TEST);
}

bool test_run_test(const char* test_name, test_func func)
{
    log_format_msg(LOG_STDOUT | LOG_TEST, "Test: %s", test_name);
    log_push_indent(LOG_STDOUT | LOG_TEST);

    bool success = func();

    log_pop_indent(LOG_STDOUT | LOG_TEST);
    log_msg(LOG_STDOUT | LOG_TEST, success ? "SUCCESS" : "FAILED");

    return success;
}
