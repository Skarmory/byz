#include "test/test.h"

#include <string.h>

bool test_assert_equal_char_buffer(const char* lhs, const char* rhs)
{
    int lhs_len = strlen(lhs);
    int rhs_len = strlen(rhs);

    if(lhs_len != rhs_len)
    {
        return TEST_CASE_FAIL;
    }

    for(int i = 0; i < lhs_len; ++i)
    {
        if(lhs[i] != rhs[i])
        {
            return TEST_CASE_FAIL;
        }
    }

    return TEST_CASE_SUCCESS;
}

bool test_assert_equal_int(const int lhs, const int rhs)
{
    return lhs == rhs;
}
