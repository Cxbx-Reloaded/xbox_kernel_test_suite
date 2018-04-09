#include <pbkit/pbkit.h>
#include "global.h"
#include "output.h"


void test_AvGetSavedDataAddress(){
    // FIXME this test is broken - We shouldn't pass NULL to AvSetSavedDataAddress. This is just an hack to avoid a system crash.
    const char* func_num = "0x0001";
    const char* func_name = "AvGetSavedDataAddress";
    BOOL tests_passed = 1;
    print_test_header(func_num, func_name);

    AvSetSavedDataAddress((void*) NULL);
    tests_passed = AvGetSavedDataAddress()==(void*) NULL ? 1 : 0;
    print("AvGetSavedDataAddress()=0x%x", AvGetSavedDataAddress());

    print_test_footer(func_num, func_name, tests_passed);
}

void test_AvSendTVEncoderOption(){
    // FIXME: there are other functions such as AV_OPTION_QUERY_MODE, AV_QUERY_ENCODER_TYPE, AV_OPTION_WIDESCREEN etc
    // FIXME: this test is broken. I get inconsistent value from my real xbox
    const char* func_num = "0x0002";
    const char* func_name = "AvSendTVEncoderOption";
    BOOL tests_passed = 1;
    print_test_header(func_num, func_name);

    unsigned long res = 0;
    AvSendTVEncoderOption((void *)NV2A_MMIO_BASE, 6, 0, &res);
    print("AvSendTVEncoderOption: %d (0=AV_PACK_NONE 1=AV_PACK_STANDARD 2=AV_PACK_RFU 3=AV_PACK_SCART 4=AV_PACK_HDTV 5=AV_PACK_VGA 6=AV_PACK_SVIDEO)", (int)res);

    print_test_footer(func_num, func_name, tests_passed);
}

void test_AvSetDisplayMode(){
    // FIXME: FATAL - Fails on emulator as of april 6th, 2018
    // FIXME: FATAL - Fails on real hardware too. The parameters passed to AvSetDisplayMode are wrong.
    return;
    AvSetDisplayMode(NULL, 0, 0, 0, 0, 0) == 0L ? print("0x0003 - AvSetDisplayMode: 0 (Good)") : print("0x0003 - AvSetDisplayMode: !=0 (Faulty)");
}

void test_AvSetSavedDataAddress(){
    // FIXME this test is broken - We shouldn't pass NULL to AvSetSavedDataAddress. This is just an hack to avoid a system crash.
    const char* func_num = "0x0004";
    const char* func_name = "AvSetSavedDataAddress";
    BOOL tests_passed = 1;
    print_test_header(func_num, func_name);

    AvSetSavedDataAddress((void*) NULL);
    tests_passed = AvGetSavedDataAddress()==(void*) NULL ? 1 : 0;

    print_test_footer(func_num, func_name, tests_passed);
}
