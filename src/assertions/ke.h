#pragma once

#include <xboxkrnl/xboxkrnl.h>

BOOL assert_critical_region(PKTHREAD, ULONG, const char*);
