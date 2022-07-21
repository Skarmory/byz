#include "test/core/string.h"

#include "core/log.h"
#include "core/string.h"

#include "test/test.h"

static const char* TEST_STRING_CONTENT = "This is the test string!";
static const int TEST_STRING_CONTENT_LEN = 24;

bool test_string_new(void)
{
    log_msg(LOG_STDOUT | LOG_TEST, "Running test: test_string_new");

    bool success = true;

    struct string* test_string = string_new(TEST_STRING_CONTENT);
    success &= test_assert_equal_char_buffer(TEST_STRING_CONTENT, test_string->buffer);
    string_free(test_string);

    log_msg(LOG_STDOUT | LOG_TEST, success ? "SUCCESS" : "FAILED");

    return success;
}

bool test_string_init(void)
{
    bool success = true;

    struct string test_string;
    string_init(&test_string, TEST_STRING_CONTENT);
    success &= test_assert_equal_char_buffer(TEST_STRING_CONTENT, test_string.buffer);

    return success;
}

bool test_string_size(void)
{
    bool success = true;

    struct string test_string;
    string_init(&test_string, TEST_STRING_CONTENT);
    success &= test_assert_equal_int(TEST_STRING_CONTENT_LEN, string_size(&test_string));

    return success;
}

void test_string_run_all_tests(void)
{
    test_string_new();
    test_string_init();
    test_string_size();
}
