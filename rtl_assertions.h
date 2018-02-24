#ifndef RTL_ASSERTIONS_H
#define RTL_ASSERTIONS_H

#include <xboxkrnl/xboxkrnl.h>

BOOL assert_critical_section_equals(
    PRTL_CRITICAL_SECTION,
    LONG,
    LONG,
    HANDLE,
    const char*
);

BOOL assert_ansi_string(
    PANSI_STRING,
    USHORT,
    USHORT,
    PCHAR,
    const char*
);

#endif // RTL_ASSERTIONS_H
