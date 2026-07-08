#pragma once

#include <xboxkrnl/xboxkrnl.h>

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(x) (sizeof(x) / sizeof(x[0]))
#endif

// Cached HLE detection flag. Must be initialized at startup via init_hle_detection().
extern BOOL g_is_hle;

// Call once during initialization to cache HLE detection result.
// Alternative detection methods:
// - Check if kernel image base (0x80010000) passes MmIsAddressValid
// - Compare XboxKrnlVersion fields against known emulator values
// - Check for known emulator-specific behavior (e.g. specific HW flags)
// The current approach (testing a known kernel export) is the most reliable
// since it directly tests the condition we care about: whether kernel
// exports reside in Xbox-managed memory.
static inline void init_hle_detection(void)
{
    g_is_hle = !MmIsAddressValid((PVOID)ExAllocatePoolWithTag);
}

// Validate a pointer to kernel image content (code or data exports).
// In HLE, kernel exports live in the host DLL outside Xbox-managed memory,
// so MmIsAddressValid will fail. We can only verify non-NULL there.
// On real hardware, the pointer must also pass MmIsAddressValid.
static inline BOOLEAN is_kernel_export_valid(PVOID ptr)
{
    if (ptr == NULL) {
        return FALSE;
    }
    if (g_is_hle) {
        return TRUE; // Non-NULL is the best we can check in HLE
    }
    return MmIsAddressValid(ptr);
}
