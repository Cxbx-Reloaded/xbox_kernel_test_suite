#include <xboxkrnl/xboxkrnl.h>

#include "assertions/ke.h"
#include "util/output.h"

void test_KeEnterCriticalRegion(int func_num, const char* func_name)
{
    BOOL tests_passed = 1;
    print_test_header(func_num, func_name);

    PKTHREAD thread = KeGetCurrentThread();
    tests_passed &= assert_critical_region(thread, 0, "Before entering critical region");

    KeEnterCriticalRegion();
    tests_passed &= assert_critical_region(thread, -1, "In critical region");

    KeEnterCriticalRegion();
    tests_passed &= assert_critical_region(thread, -2, "Entered critical region again");

    KeLeaveCriticalRegion();
    KeLeaveCriticalRegion();
    tests_passed &= assert_critical_region(thread, 0, "Leave critical region twice");

    print_test_footer(func_num, func_name, tests_passed);
}
