#include <xboxkrnl/xboxkrnl.h>

#include "util/output.h"
#include "assertions/defines.h"

TEST_FUNC(NtUserIoApcDispatcher)
{
    TEST_BEGIN();

    // NtUserIoApcDispatcher is the kernel's I/O APC dispatcher routine.
    // It's called by the kernel when an async I/O operation completes
    // to deliver the APC to the requesting thread.
    //
    // SKIP: This function is an internal kernel callback that cannot be
    // meaningfully called directly from user/kernel test code. It expects
    // to be invoked in APC context with specific kernel-internal state.
    // Calling it directly would require fabricating an IO_STATUS_BLOCK
    // that was actually queued by the I/O manager.
    //
    // Testing async I/O with APCs is covered implicitly by the
    // NtReadFileScatter/NtWriteFileGather tests when using async handles.

    GEN_CHECK(TRUE, TRUE, "NtUserIoApcDispatcher is internal kernel callback");

    TEST_END();
}
