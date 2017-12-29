#ifndef IO_TESTS_H
#define IO_TESTS_H

void test_IoAllocateIrp();
void test_IoBuildAsynchronousFsdRequest();
void test_IoBuildDeviceIoControlRequest();
void test_IoBuildSynchronousFsdRequest();
void test_IoCheckShareAccess();
void test_IoCompletionObjectType();
void test_IoCreateDevice();
void test_IoCreateFile();
void test_IoCreateSymbolicLink();
void test_IoDeleteDevice();
void test_IoDeleteSymbolicLink();
void test_IoDeviceObjectType();
void test_IoFileObjectType();
void test_IoFreeIrp();
void test_IoInitializeIrp();
void test_IoInvalidDeviceRequest();
void test_IoQueryFileInformation();
void test_IoQueryVolumeInformation();
void test_IoQueueThreadIrp();
void test_IoRemoveShareAccess();
void test_IoSetIoCompletion();
void test_IoSetShareAccess();
void test_IoStartNextPacket();
void test_IoStartNextPacketByKey();
void test_IoStartPacket();
void test_IoSynchronousDeviceIoControlRequest();
void test_IoSynchronousFsdRequest();
void test_IoDismountVolume();
void test_IoDismountVolumeByName();
void test_IoMarkIrpMustComplete();


#endif
