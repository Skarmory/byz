#include "test/core/string.h"

#include "core/log.h"

int main(int argc, char** argv)
{
    init_logs();

    test_string_run_all_tests();

    uninit_logs();

    return 0;
}
