#include <xboxkrnl/xboxkrnl.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <ctype.h>

#include "global.h" // for seed var
#include "util/output.h"
#include "util/misc.h"
#include "assertions/defines.h"

TEST_FUNC(RtlUpcaseUnicodeChar)
{
    TEST_BEGIN();

    typedef struct _upcase_test {
        WCHAR input;
        WCHAR expected_output;
        WCHAR return_result;
    } upcase_test;

    upcase_test tests[] = {
        { .input = L' ', .expected_output = L' ' },
        { .input = L'a', .expected_output = L'A' },
        { .input = L'z', .expected_output = L'Z' },
        { .input = L'A', .expected_output = L'A' },
        { .input = L'Z', .expected_output = L'Z' },
        { .input = L'0', .expected_output = L'0' },
        { .input = L'$', .expected_output = L'$' },
    };

    for (unsigned i = 0; i < ARRAY_SIZE(tests); i++) {
        tests[i].return_result = RtlUpcaseUnicodeChar(tests[i].input);
    }
    GEN_CHECK_ARRAY_MEMBER(tests, return_result, expected_output, ARRAY_SIZE(tests), "upcase_tests");

    TEST_END();
}

TEST_FUNC(RtlUpcaseUnicodeString)
{
    TEST_BEGIN();

    UNICODE_STRING src_str, dest_str;
    WCHAR dest_buf[32];
    NTSTATUS status;

    // Test in-place upcase (AllocateDestinationString = FALSE)
    dest_str.Buffer = dest_buf;
    dest_str.MaximumLength = sizeof(dest_buf);
    dest_str.Length = 0;

    RtlInitUnicodeString(&src_str, L"xbox");
    status = RtlUpcaseUnicodeString(&dest_str, &src_str, FALSE);
    GEN_CHECK(status, STATUS_SUCCESS, "upcase status");
    GEN_CHECK(dest_str.Length, 8, "upcase Length");
    if (dest_str.Buffer) {
        GEN_CHECK(dest_str.Buffer[0], L'X', "Buffer[0]");
        GEN_CHECK(dest_str.Buffer[1], L'B', "Buffer[1]");
        GEN_CHECK(dest_str.Buffer[2], L'O', "Buffer[2]");
        GEN_CHECK(dest_str.Buffer[3], L'X', "Buffer[3]");
    }

    // Test with AllocateDestinationString = TRUE
    UNICODE_STRING alloc_dest = { 0 };
    RtlInitUnicodeString(&src_str, L"hello");
    status = RtlUpcaseUnicodeString(&alloc_dest, &src_str, TRUE);
    GEN_CHECK(status, STATUS_SUCCESS, "alloc upcase status");
    if (NT_SUCCESS(status)) {
        GEN_CHECK(alloc_dest.Length, 10, "alloc Length");
        if (alloc_dest.Buffer) {
            GEN_CHECK(alloc_dest.Buffer[0], L'H', "alloc Buffer[0]");
            GEN_CHECK(alloc_dest.Buffer[4], L'O', "alloc Buffer[4]");
        }
        RtlFreeUnicodeString(&alloc_dest);
    }

    TEST_END();
}

TEST_FUNC(RtlUpcaseUnicodeToMultiByteN)
{
    TEST_BEGIN();

    WCHAR src[] = L"xbox";
    CHAR dest[16] = { 0 };
    ULONG bytes_written = 0;
    NTSTATUS status;

    // Convert unicode to multibyte with upcase
    status = RtlUpcaseUnicodeToMultiByteN(dest, sizeof(dest), &bytes_written, src, 8);
    GEN_CHECK(status, STATUS_SUCCESS, "upcase multi status");
    GEN_CHECK(bytes_written, 4, "bytes_written");
    GEN_CHECK(dest[0], 'X', "dest[0]");
    GEN_CHECK(dest[1], 'B', "dest[1]");
    GEN_CHECK(dest[2], 'O', "dest[2]");
    GEN_CHECK(dest[3], 'X', "dest[3]");

    // Test with limited buffer
    RtlZeroMemory(dest, sizeof(dest));
    bytes_written = 0;
    status = RtlUpcaseUnicodeToMultiByteN(dest, 2, &bytes_written, src, 8);
    GEN_CHECK(status, STATUS_BUFFER_OVERFLOW, "overflow status");
    GEN_CHECK(bytes_written, 2, "limited bytes_written");
    GEN_CHECK(dest[0], 'X', "limited dest[0]");
    GEN_CHECK(dest[1], 'B', "limited dest[1]");

    TEST_END();
}

TEST_FUNC(RtlUpperChar)
{
    TEST_BEGIN();

    // These results are taken from running inputs 0 through 255 on a NTSC Xbox
    CHAR expected_outputs[] = {
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
        0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F,
        0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F,
        0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F,
        0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F,
        0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5A, 0x5B, 0x5C, 0x5D, 0x5E, 0x5F,
        0x60, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F,
        0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5A, 0x7B, 0x7C, 0x7D, 0x7E, 0x7F,
        0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8A, 0x8B, 0x8C, 0x8D, 0x8E, 0x8F,
        0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9A, 0x9B, 0x9C, 0x9D, 0x9E, 0x9F,
        0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7, 0xA8, 0xA9, 0xAA, 0xAB, 0xAC, 0xAD, 0xAE, 0xAF,
        0xB0, 0xB1, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0xB9, 0xBA, 0xBB, 0xBC, 0xBD, 0xBE, 0xBF,
        0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xCF,
        0xD0, 0xD1, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7, 0xD8, 0xD9, 0xDA, 0xDB, 0xDC, 0xDD, 0xDE, 0xDF,
        0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xCF,
        0xD0, 0xD1, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xF7, 0xD8, 0xD9, 0xDA, 0xDB, 0xDC, 0xDD, 0xDE, 0x3F,
    };
    enum { expected_length = ARRAY_SIZE(expected_outputs) };
    CHAR results[expected_length];

    for (unsigned i = 0; i < expected_length; i++) {
        results[i] = RtlUpperChar((CHAR)i);
    }
    GEN_CHECK_ARRAY(results, expected_outputs, expected_length, "results");

    TEST_END();
}

TEST_FUNC(RtlUpperString)
{
    TEST_BEGIN();

    char rnd_letter;
    char rnd_letters[101];
    char rnd_letters_upper[101];

    srand(seed);
    for (int k = 0; k < 100; k++) {
        rnd_letter = "abcdefghijklmnopqrstuvwxyz"[rand() % 26];
        rnd_letters[k] = rnd_letter;
        rnd_letters_upper[k] = toupper(rnd_letter);
    }
    rnd_letters[100] = '\0';
    rnd_letters_upper[100] = '\0';

    ANSI_STRING src_str;
    ANSI_STRING res_str;
    char res_buf[256];

    /* Initialize res_buf so RtlInitAnsiString works correctly */
    for (int i = 0; i < 255; i++)
        res_buf[i] = '0';
    res_buf[255] = 0;

    RtlInitAnsiString(&res_str, res_buf);

    /* Empty String Test */
    RtlInitAnsiString(&src_str, "");
    RtlUpperString(&res_str, &src_str);
    GEN_CHECK(strncmp(res_str.Buffer, "", res_str.Length) == 0, TRUE, "empty");

    /* Lowercase String Test */
    RtlInitAnsiString(&src_str, "xbox");
    RtlUpperString(&res_str, &src_str);
    GEN_CHECK(strncmp(res_str.Buffer, "XBOX", res_str.Length) == 0, TRUE, "compare");

    /* Lowercase Single Character Test */
    RtlInitAnsiString(&src_str, "x");
    RtlUpperString(&res_str, &src_str);
    GEN_CHECK(strncmp(res_str.Buffer, "X", res_str.Length) == 0, TRUE, "compare");

    /* Uppercase Single Character Test */
    RtlInitAnsiString(&src_str, "X");
    RtlUpperString(&res_str, &src_str);
    GEN_CHECK(strncmp(res_str.Buffer, "X", res_str.Length) == 0, TRUE, "compare");

    /* 100 Lowercase Characters Test */
    RtlInitAnsiString(&src_str, rnd_letters);
    RtlUpperString(&res_str, &src_str);
    GEN_CHECK_ARRAY(res_str.Buffer, rnd_letters_upper, ARRAY_SIZE(rnd_letters_upper) - 1, "compare");

    /* Uppercase String Test */
    RtlInitAnsiString(&src_str, "XBOX");
    RtlUpperString(&res_str, &src_str);
    GEN_CHECK(strncmp(res_str.Buffer, "XBOX", res_str.Length) == 0, TRUE, "compare");

    TEST_END();
}
