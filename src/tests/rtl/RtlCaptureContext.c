#include <xboxkrnl/xboxkrnl.h>
#include <stdint.h>

#include "global.h" // for (passed|failed)_test vars
#include "util/output.h"

// Notes about this function: The official kernel allocates 0x440 bytes for the CONTEXT structure.
// That does not match with any other implementation of CONTEXT that I have been able to find.
// RtlCaptureContext does not touch the ContextFlags or Floating point save fields of the CONTEXT
// structure.
//
// FIXME: RtlCaptureContext also sets Ebp, Eip, and Esp in the context record. No checks are enabled for them
// currently.
void test_RtlCaptureContext(){
    const char* func_num = "0x0109";
    const char* func_name = "RtlCaptureContext";
    BOOL tests_passed = 1;
    print_test_header(func_num, func_name);

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

    __asm__ __volatile__(
        // Make sure we can clobber all registers
        "pusha\n"
        // Get address of RtlCaptureContext and store it for later
        "movl __imp__RtlCaptureContext@4, %%eax\n"
        "movl %%eax, %0\n"
        // Push &result_context to pass into RtlCaptureContext
        "leal %1, %%eax\n"
        "pushl %%eax\n"
        // Store current values of cs and ss
        "leal %2, %%eax\n"
        "movw %%cs, (%%eax)\n"
        "leal %3, %%eax\n"
        "movw %%ss, (%%eax)\n"
        // Store current value of the flags
        "leal %4, %%eax\n"
        "pushf\n"
        "popl (%%eax)\n"
        // Setup pre-set register values
        "movl %5, %%eax\n"
        "movl %6, %%ebx\n"
        "movl %7, %%ecx\n"
        "movl %8, %%edx\n"
        "movl %9, %%esi\n"
        "movl %10, %%edi\n"
        // Call RtlCaptureContext and cleanup registers
        "calll *%11\n"
        "popa\n"
        : "=m" (RtlCaptureContext_address)
        : "m" (result_context), "m" (cs_val), "m" (ss_val), "m" (flags_val),
          "m" (eax_val), "m" (ebx_val), "m" (ecx_val), "m" (edx_val), "m" (esi_val),
          "m" (edi_val), "m" (RtlCaptureContext_address)
    );

    DWORD* expected_vals[]  = {&eax_val, &ebx_val, &ecx_val, &edx_val, &esi_val, &edi_val,
                               &cs_val, &ss_val, &flags_val};
    DWORD* result_vals[]    = {&result_context.Eax, &result_context.Ebx, &result_context.Ecx,
                               &result_context.Edx, &result_context.Esi, &result_context.Edi,
                               &result_context.SegCs, &result_context.SegSs, &result_context.EFlags};
    const char* reg_names[] = {"Eax", "Ebx", "Ecx", "Edx", "Esi", "Edi", "SegCs", "SegSs", "EFlags"};

    for(uint8_t i = 0; i < sizeof(expected_vals) / sizeof(DWORD*); i++) {
        const char* result_text = passed_text;
        if(*result_vals[i] != *expected_vals[i]) {
            tests_passed = 0;
            result_text = failed_text;
        }
        print("  Test %s Expected %s = 0x%x, got = 0x%x", result_text, reg_names[i], *expected_vals[i], *result_vals[i]);
    }

    print_test_footer(func_num, func_name, tests_passed);
}
