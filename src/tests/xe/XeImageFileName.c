#include <xboxkrnl/xboxkrnl.h>
#include <string.h>

#include "util/output.h"
#include "assertions/defines.h"

TEST_FUNC(XeImageFileName)
{
    TEST_BEGIN();

    // XeImageFileName is a kernel-exported ANSI_STRING containing the path
    // to the currently running XBE image.

    // It should be non-NULL
    PANSI_STRING pName = (PANSI_STRING)XeImageFileName;
    BOOL valid_ptr = (pName != NULL);
    GEN_CHECK(valid_ptr, TRUE, "XeImageFileName not NULL");

    if (pName != NULL) {
        // Length must be > 0 (a running XBE always has a path)
        BOOL has_length = (pName->Length > 0);
        GEN_CHECK(has_length, TRUE, "Length > 0");

        // MaximumLength must be >= Length
        BOOL max_ge_len = (pName->MaximumLength >= pName->Length);
        GEN_CHECK(max_ge_len, TRUE, "MaximumLength >= Length");

        // Buffer must be non-NULL
        BOOL has_buffer = (pName->Buffer != NULL);
        GEN_CHECK(has_buffer, TRUE, "Buffer not NULL");

        if (pName->Buffer != NULL && pName->Length > 0) {
            // The path should contain a backslash (Xbox paths are like \Device\...)
            BOOL has_backslash = (memchr(pName->Buffer, '\\', pName->Length) != NULL);
            GEN_CHECK(has_backslash, TRUE, "path contains backslash");

            // The path should end with .xbe (case insensitive not required for kernel)
            // At minimum it should have some recognizable extension or path component
            BOOL reasonable_len = (pName->Length >= 5); // e.g. "x.xbe" minimum
            GEN_CHECK(reasonable_len, TRUE, "path length >= 5");

            // ANSI_STRING buffers are NOT guaranteed to be null-terminated.
            // The authoritative length is the Length member, not a null terminator.
            // Verify no embedded nulls exist within the declared length, which would
            // indicate corruption or a malformed string.
            BOOL no_embedded_null = (memchr(pName->Buffer, '\0', pName->Length) == NULL);
            GEN_CHECK(no_embedded_null, TRUE, "no embedded nulls");
        }
    }

    TEST_END();
}
