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
    log_msg(LOG_STDOUT | LOG_TEST, "Running test: test_string_init");

    bool success = true;

    struct string test_string;
    string_init(&test_string, TEST_STRING_CONTENT);
    success &= test_assert_equal_char_buffer(TEST_STRING_CONTENT, test_string.buffer);

    string_uninit(&test_string);

    log_msg(LOG_STDOUT | LOG_TEST, success ? "SUCCESS" : "FAILED");

    return success;
}

bool test_string_size(void)
{
    log_msg(LOG_STDOUT | LOG_TEST, "Running test: test_string_size");

    bool success = true;

    struct string test_string;
    string_init(&test_string, TEST_STRING_CONTENT);
    success &= test_assert_equal_int(TEST_STRING_CONTENT_LEN, string_size(&test_string));

    string_uninit(&test_string);

    log_msg(LOG_STDOUT | LOG_TEST, success ? "SUCCESS" : "FAILED");

    return success;
}

static bool _test_string_format_large_buffer(void)
{
    const char* format_string = "%s %d %.2f";
    const char* format_test_string = "Hello";
    const int format_test_int = 7;
    const float format_test_float = 97.0f;

    const char* expected = "Hello 7 97.0";

    struct string test_string;
    string_init(&test_string, "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789");
    string_format(&test_string, format_string, format_test_string, format_test_int, format_test_float);

    bool success = test_assert_equal_char_buffer(expected, test_string.buffer);

    string_uninit(&test_string);

    return success;
}

static bool _test_string_format_from_small_buffer(void)
{
    bool success = true;
    const char* format_string = "%s %d %.2f";
    const char* format_test_string = "Hello";
    const int format_test_int = 7;
    const float format_test_float = 97.0f;

    const char* expected = "Hello 7 97.0";

    struct string test_string;
    string_init(&test_string, "a");
    string_format(&test_string, format_string, format_test_string, format_test_int, format_test_float);
    success &= test_assert_equal_char_buffer(expected, test_string.buffer);

    string_uninit(&test_string);

    return success;
}

bool test_string_format(void)
{
    bool success = true;
    success &= test_run_test("small string buffer", &_test_string_format_from_small_buffer);
    success &= test_run_test("large string buffer", &_test_string_format_large_buffer);

    return success;
}

void test_string_run_all_tests(void)
{
    test_string_new();
    test_string_init();
    test_string_size();
    test_run_test_block("string_format", &test_string_format);
}
