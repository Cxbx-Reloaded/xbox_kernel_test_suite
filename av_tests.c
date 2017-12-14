#include <pbkit/pbkit.h>
#include "output.h"

void test_AvGetSavedDataAddress(){
    AvSetSavedDataAddress((void*) 0x12345678);
    AvGetSavedDataAddress()==(void*)0x12345678 ? print("0x0001 - AvGetSavedDataAddress: Good") : print("0x0001 - AvGetSavedDataAddress: Faulty");
}

void test_AvSendTVEncoderOption(){
    unsigned long res = 0;
    AvSendTVEncoderOption(NULL, 6, 0, &res);
    print("0x0002 - AvSendTVEncoderOption: %d (0=AV_PACK_NONE 1=AV_PACK_STANDARD 2=AV_PACK_RFU 3=AV_PACK_SCART 4=AV_PACK_HDTV 5=AV_PACK_VGA 6=AV_PACK_SVIDEO)", res);
}

void test_AvSetDisplayMode(){
    AvSetDisplayMode(NULL, 0, 0, 0, 0, 0) == 0L ? print("0x0003 - AvSetDisplayMode: 0 (Good)") : print("0x0003 - AvSetDisplayMode: !=0 (Faulty)");
}

void test_AvSetSavedDataAddress(){
    AvSetSavedDataAddress((void*) 0x12345678);
    AvGetSavedDataAddress()==(void*)0x12345678 ? print("0x0004 - AvSetSavedDataAddress: Good") : print("0x0004 - AvSetSavedDataAddress: Faulty");
}
