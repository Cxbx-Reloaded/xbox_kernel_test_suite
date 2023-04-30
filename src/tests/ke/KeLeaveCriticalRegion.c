#include <xboxkrnl/xboxkrnl.h>

#include "assertions/ke.h"
#include "util/output.h"

void test_KeLeaveCriticalRegion(){
    const char* func_num = "0x007A";
    const char* func_name = "KeLeaveCriticalRegion";
    BOOL tests_passed = 1;
    print_test_header(func_num, func_name);

    PKTHREAD thread = KeGetCurrentThread();
    KeEnterCriticalRegion();
    KeEnterCriticalRegion();

    KeLeaveCriticalRegion();
    tests_passed &= assert_critical_region(thread, -1, "Leave critical region after entering twice");

    KeLeaveCriticalRegion();
    tests_passed &= assert_critical_region(thread, 0, "Leave critical region again");

    print_test_footer(func_num, func_name, tests_passed);
}
