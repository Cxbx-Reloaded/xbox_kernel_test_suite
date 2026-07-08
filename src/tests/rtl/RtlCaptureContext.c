#include <xboxkrnl/xboxkrnl.h>

#include "global.h" // for (passed|failed)_test vars
#include "assertions/defines.h"
#include "util/output.h"
#include "util/misc.h"

// Notes about this function: The official kernel allocates 0x440 bytes for the CONTEXT structure.
// That does not match with any other implementation of CONTEXT that I have been able to find.
// RtlCaptureContext does not touch the ContextFlags or Floating point save fields of the CONTEXT
// structure.
TEST_FUNC(RtlCaptureContext)
{
    TEST_BEGIN();

    CONTEXT result_context;
    // cs and ss words so make sure that the upper word of the Seg* dword is 0 or the tests will fail
    // due to unitialized values.
    result_context.SegCs = 0;
    result_context.SegSs = 0;

    DWORD eax_val = 0x336699CC;
    DWORD ebx_val = 0x11223344;
    DWORD ecx_val = 0x55667788;
    DWORD edx_val = 0x99AABBCC;
    DWORD esi_val = 0xDDEEFF11;
    DWORD edi_val = 0xDEADBEEF;
    DWORD flags_val = 0;
    // Again, cs and ss are words so make sure the upper word is initialized to 0
    DWORD cs_val = 0;
    DWORD ss_val = 0;
    DWORD RtlCaptureContext_address = 0;
    DWORD expected_ebp = 0;
    DWORD expected_esp = 0;
    DWORD expected_eip = 0;

    __asm__ __volatile__(
        // Make sure we can clobber all registers
        "pusha\n"
        // Get address of RtlCaptureContext and store it for later
        "movl __imp__RtlCaptureContext@4, %%eax\n"
        "movl %%eax, %0\n"
        // Push &result_context to pass into RtlCaptureContext
        "leal %3, %%eax\n"
        "pushl %%eax\n"
        // Store current values of cs and ss
        "leal %4, %%eax\n"
        "movw %%cs, (%%eax)\n"
        "leal %5, %%eax\n"
        "movw %%ss, (%%eax)\n"
        // Store current value of the flags
        "leal %6, %%eax\n"
        "pushf\n"
        "popl (%%eax)\n"
        // Capture Ebp (we don't modify it, so it's the frame pointer throughout)
        "movl %%ebp, %1\n"
        // Capture Esp as it will be after the param push is cleaned up by stdcall.
        // RtlCaptureContext(@4) pops the param on ret, so caller's Esp = Esp before pushl.
        // We already pushed the param, so caller's Esp = current Esp + 4.
        "leal 4(%%esp), %%eax\n"
        "movl %%eax, %2\n"
        // Setup pre-set register values
        "movl %7, %%eax\n"
        "movl %8, %%ebx\n"
        "movl %9, %%ecx\n"
        "movl %10, %%edx\n"
        "movl %11, %%esi\n"
        "movl %12, %%edi\n"
        // Call RtlCaptureContext - Eip in context will be the return address (0f label)
        "calll *%13\n"
        "0:\n"
        // Capture the return address (where Eip should point)
        "leal 0b, %%eax\n"
        "movl %%eax, %14\n"
        "popa\n"
        : "=m" (RtlCaptureContext_address), "=m" (expected_ebp), "=m" (expected_esp)
        : "m" (result_context), "m" (cs_val), "m" (ss_val), "m" (flags_val),
          "m" (eax_val), "m" (ebx_val), "m" (ecx_val), "m" (edx_val), "m" (esi_val),
          "m" (edi_val), "m" (RtlCaptureContext_address), "m" (expected_eip)
    );

    typedef struct _context_test {
        const DWORD* expected;
        const DWORD* actual;
        const char* const reg_name;
    } context_test;

    context_test context_tests[] = {
        { .expected = &eax_val, .actual = &result_context.Eax, .reg_name = "Eax" },
        { .expected = &ebx_val, .actual = &result_context.Ebx, .reg_name = "Ebx" },
        { .expected = &ecx_val, .actual = &result_context.Ecx, .reg_name = "Ecx" },
        { .expected = &edx_val, .actual = &result_context.Edx, .reg_name = "Edx" },
        { .expected = &esi_val, .actual = &result_context.Esi, .reg_name = "Esi" },
        { .expected = &edi_val, .actual = &result_context.Edi, .reg_name = "Edi" },
        { .expected = &cs_val, .actual = &result_context.SegCs, .reg_name = "SegCs" },
        { .expected = &ss_val, .actual = &result_context.SegSs, .reg_name = "SegSs" },
        { .expected = &flags_val, .actual = &result_context.EFlags, .reg_name = "EFlags" },
        { .expected = &expected_ebp, .actual = &result_context.Ebp, .reg_name = "Ebp" },
        { .expected = &expected_esp, .actual = &result_context.Esp, .reg_name = "Esp" },
        { .expected = &expected_eip, .actual = &result_context.Eip, .reg_name = "Eip" }
    };

    for (unsigned i = 0; i < ARRAY_SIZE(context_tests); i++) {
        GEN_CHECK(*context_tests[i].actual, *context_tests[i].expected, context_tests[i].reg_name);
    }

    TEST_END();
}
