#pragma once

#include "defines.h"

#define assert_critical_region_ex( \
    thread, \
    expected_Apc, \
    line_number \
) \
    GEN_CHECK_EX(thread->KernelApcDisable, expected_Apc, ".KernelApcDisable", line_number)
#define assert_critical_region( \
    thread, \
    expected_Apc \
) \
assert_critical_region_ex( \
    thread, \
    expected_Apc, \
    __LINE__ \
)
