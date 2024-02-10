#pragma once

void test_XcSHAInit(int, const char*);
void test_XcSHAUpdate(int, const char*);
void test_XcSHAFinal(int, const char*);
void test_XcRC4Key(int, const char*);
void test_XcRC4Crypt(int, const char*);
void test_XcHMAC(int, const char*);
void test_XcPKEncPublic(int, const char*);
void test_XcPKDecPrivate(int, const char*);
void test_XcPKGetKeyLen(int, const char*);
void test_XcVerifyPKCS1Signature(int, const char*);
void test_XcModExp(int, const char*);
void test_XcDESKeyParity(int, const char*);
void test_XcKeyTable(int, const char*);
void test_XcBlockCrypt(int, const char*);
void test_XcBlockCryptCBC(int, const char*);
void test_XcCryptService(int, const char*);
void test_XcUpdateCrypto(int, const char*);
