#include <xboxkrnl/xboxkrnl.h>

#include "util/output.h"

void test_AvSetDisplayMode(){
    // FIXME: FATAL - Fails on emulator as of april 6th, 2018
    // FIXME: FATAL - Fails on real hardware too. The parameters passed to AvSetDisplayMode are wrong.
    return;
    AvSetDisplayMode(NULL, 0, 0, 0, 0, 0) == 0L ? print("0x0003 - AvSetDisplayMode: 0 (Good)") : print("0x0003 - AvSetDisplayMode: !=0 (Faulty)");
}
