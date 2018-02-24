#ifndef KE_ASSERTIONS_H
#define KE_ASSERTIONS_H

#include <xboxkrnl/xboxkrnl.h>

BOOL assert_critical_region(PKTHREAD, ULONG, const char*);

#endif // KE_ASSERTIONS_H
