#include "test/core/string.h"
#include "test/core/list.h"

#include "core/log.h"

int main(int argc, char** argv)
{
    init_logs();

    test_string_run_all_tests();
    test_list_run_all_tests();

    uninit_logs();

    return 0;
}
