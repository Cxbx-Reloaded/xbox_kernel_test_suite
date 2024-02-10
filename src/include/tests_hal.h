#pragma once

void test_HalReadSMCTrayState(int, const char*);
void test_HalClearSoftwareInterrupt(int, const char*);
void test_HalDisableSystemInterrupt(int, const char*);
void test_HalDiskCachePartitionCount(int, const char*);
void test_HalDiskModelNumber(int, const char*);
void test_HalDiskSerialNumber(int, const char*);
void test_HalEnableSystemInterrupt(int, const char*);
void test_HalGetInterruptVector(int, const char*);
void test_HalReadSMBusValue(int, const char*);
void test_HalReadWritePCISpace(int, const char*);
void test_HalRegisterShutdownNotification(int, const char*);
void test_HalRequestSoftwareInterrupt(int, const char*);
void test_HalReturnToFirmware(int, const char*);
void test_HalWriteSMBusValue(int, const char*);
void test_HalBootSMCVideoMode(int, const char*);
void test_HalIsResetOrShutdownPending(int, const char*);
void test_HalInitiateShutdown(int, const char*);
void test_HalEnableSecureTrayEject(int, const char*);
void test_HalWriteSMCScratchRegister(int, const char*);
