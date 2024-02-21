#include <xboxkrnl/xboxkrnl.h>
#include <string.h>

#include "util/output.h"
#include "assertions/xc.h"

TEST_FUNC(XcBlockCrypt)
{
	TEST_BEGIN();

	ULONG cipher_selector;
	UCHAR input_buffer[8]       = { 0x54,0x65,0x73,0x74 }; // "Test"
	UCHAR output_buffer[8]      = { 0 };
	UCHAR key_table_des[128]    = { 0 };
	UCHAR key_table_3des[128*3] = { 0 };
	UCHAR key_des[8]            = { 0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17 };
	UCHAR key_3des[8*3]         = {
		0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,
		0x20,0x21,0x22,0x23,0x24,0x25,0x26,0x27,
		0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37
	};

	// DES encrypt
	XcDESKeyParity(key_des, sizeof(key_des));
	cipher_selector = 0;
	XcKeyTable(cipher_selector, key_table_des, key_des);
	XcBlockCrypt(cipher_selector, output_buffer, input_buffer, key_table_des, 1);
	assert_hashed_result(output_buffer,
	                     sizeof(output_buffer),
	                     (PUCHAR)"\x2a\x6c\x52\x95\x2c\x94\x23\xe7\x88\x30\x72\x85\xd4\xca\x40\xe4\x56\xe8\xb8\x60",
	                     "DES encrypt");

	// DES decrypt
	memcpy(input_buffer, output_buffer, sizeof(input_buffer));
	memset(output_buffer, 0, sizeof(output_buffer));
	XcBlockCrypt(cipher_selector, output_buffer, input_buffer, key_table_des, 0);
	assert_hashed_result(output_buffer,
	                     sizeof(output_buffer),
	                     (PUCHAR)"\x40\x34\x58\xa2\x26\xff\x6b\x3e\xa6\x49\xde\xc2\x34\x05\x7a\xdf\x58\x57\x38\x4a",
	                     "DES decrypt");

	// Bogus operation value (DES)
	memcpy(input_buffer, output_buffer, sizeof(input_buffer));
	memset(output_buffer, 0, sizeof(output_buffer));
	XcBlockCrypt(cipher_selector, output_buffer, input_buffer, key_table_des, 0xFFFFFFFF); 
	assert_hashed_result(output_buffer,
	                     sizeof(output_buffer),
	                     (PUCHAR)"\x2a\x6c\x52\x95\x2c\x94\x23\xe7\x88\x30\x72\x85\xd4\xca\x40\xe4\x56\xe8\xb8\x60",
	                     "DES bogus operation");

	// 3DES encrypt
	memset(input_buffer, 0, sizeof(input_buffer));
	memcpy(input_buffer, "\x54\x65\x73\x74", 4);
	memset(output_buffer, 0, sizeof(output_buffer));
	XcDESKeyParity(key_3des, sizeof(key_3des));
	cipher_selector = 1;
	XcKeyTable(cipher_selector, key_table_3des, key_3des);
	XcBlockCrypt(cipher_selector, output_buffer, input_buffer, key_table_3des, 1);
	assert_hashed_result(output_buffer,
	                     sizeof(output_buffer),
	                     (PUCHAR)"\x39\xfd\x7a\xbf\x54\xd2\x59\x30\xbd\x76\xf1\xe3\x75\xe0\xb4\x95\xea\x49\x33\x60",
	                     "3DES encrypt");

	// 3DES decrypt
	memcpy(input_buffer, output_buffer, sizeof(input_buffer));
	memset(output_buffer, 0, sizeof(output_buffer));
	XcBlockCrypt(cipher_selector, output_buffer, input_buffer, key_table_3des, 0);
	assert_hashed_result(output_buffer,
	                     sizeof(output_buffer),
	                     (PUCHAR)"\x40\x34\x58\xa2\x26\xff\x6b\x3e\xa6\x49\xde\xc2\x34\x05\x7a\xdf\x58\x57\x38\x4a",
	                     "3DES decrypt");

	// Bogus operation value (3DES)
	memset(input_buffer, 0, sizeof(input_buffer));
	memcpy(input_buffer, "\x54\x65\x73\x74", 4);
	memset(output_buffer, 0, sizeof(output_buffer));
	XcBlockCrypt(cipher_selector, output_buffer, input_buffer, key_table_3des, 0xFFFFFFFF);
	assert_hashed_result(output_buffer,
	                     sizeof(output_buffer),
	                     (PUCHAR)"\xda\x5d\x18\x4f\xd1\x0f\x2e\x33\xa1\xd1\x29\xfe\x8b\x29\xc2\x7a\x48\x81\x3c\xa2",
	                     "3DES bogus operation");

	// Bogus cipher selector value (3DES encrypt)
	memset(input_buffer, 0, sizeof(input_buffer));
	memcpy(input_buffer, "\x54\x65\x73\x74", 4);
	memset(output_buffer, 0, sizeof(output_buffer));
	cipher_selector = 0xFFFFFFFF;
	XcBlockCrypt(cipher_selector, output_buffer, input_buffer, key_table_3des, 1);
	assert_hashed_result(output_buffer,
	                     sizeof(output_buffer),
	                     (PUCHAR)"\x39\xfd\x7a\xbf\x54\xd2\x59\x30\xbd\x76\xf1\xe3\x75\xe0\xb4\x95\xea\x49\x33\x60",
	                     "bogus cipher selector (3DES encrypt)");

	// Bogus cipher selector value (3DES decrypt)
	memcpy(input_buffer, output_buffer, sizeof(input_buffer));
	memset(output_buffer, 0, sizeof(output_buffer));
	XcBlockCrypt(cipher_selector, output_buffer, input_buffer, key_table_3des, 0);
	assert_hashed_result(output_buffer,
	                     sizeof(output_buffer),
	                     (PUCHAR)"\x40\x34\x58\xa2\x26\xff\x6b\x3e\xa6\x49\xde\xc2\x34\x05\x7a\xdf\x58\x57\x38\x4a",
	                     "bogus cipher selector (3DES decrypt)");

	TEST_END();
}

TEST_FUNC(XcBlockCryptCBC)
{
	TEST_BEGIN();

	ULONG cipher_selector;
	UCHAR input_buffer[8*5]     = { 0x53,0x65,0x63,0x72,0x65,0x74,0x20,0x73,0x74,0x72,0x69,0x6e,0x67 }; // "Secret string"
	UCHAR output_buffer[8*5]    = { 0 };
	UCHAR iv[8]                 = { 0xcc,0xcc,0xcc,0xcc,0xcc,0xcc,0xcc,0xcc };
	UCHAR key_table_des[128]    = { 0 };
	UCHAR key_table_3des[128*3] = { 0 };
	UCHAR key_des[8]            = { 0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17 };
	UCHAR key_3des[8*3]         = {
		0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,
		0x20,0x21,0x22,0x23,0x24,0x25,0x26,0x27,
		0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37
	};

	// DES encrypt
	XcDESKeyParity(key_des, sizeof(key_des));
	cipher_selector = 0;
	XcKeyTable(cipher_selector, key_table_des, key_des);
	XcBlockCryptCBC(cipher_selector, sizeof(input_buffer), output_buffer, input_buffer, key_table_des, 1, iv);
	assert_hashed_result(output_buffer,
	                     sizeof(output_buffer),
	                     (PUCHAR)"\x70\xd9\x06\xb7\x88\x89\xca\x0a\x1f\x10\xa7\xcc\xc0\x5f\xf3\xfd\xa3\xd0\x9e\x79",
	                     "DES encrypt");

	// DES decrypt
	memcpy(input_buffer, output_buffer, sizeof(input_buffer));
	memset(output_buffer, 0, sizeof(output_buffer));
	memset(iv, 0xcc, sizeof(iv));
	XcBlockCryptCBC(cipher_selector, sizeof(input_buffer), output_buffer, input_buffer, key_table_des, 0, iv);
	assert_hashed_result(output_buffer,
	                     sizeof(output_buffer),
	                     (PUCHAR)"\x8f\x11\x6d\xc7\x6a\xf3\xb1\x28\xa2\xca\x83\x0f\x1e\x1f\xe3\xc8\x4a\xe1\xe3\xce",
	                     "DES decrypt");

	// DES decrypt plaintext
	memset(input_buffer, 0, sizeof(input_buffer));
	memcpy(input_buffer, "\x53\x65\x63\x72\x65\x74\x20\x73\x74\x72\x69\x6e\x67", 13);
	memset(output_buffer, 0, sizeof(output_buffer));
	memset(iv, 0xcc, sizeof(iv));
	XcBlockCryptCBC(cipher_selector, sizeof(input_buffer), output_buffer, input_buffer, key_table_des, 0, iv);
	assert_hashed_result(output_buffer,
	                     sizeof(output_buffer),
	                     (PUCHAR)"\x1f\x28\xfc\x62\x17\xc2\x46\xa5\x10\x8c\xf1\xd8\xc5\x7a\xf6\x24\x30\xf6\x48\xf5",
	                     "DES decyption plaintext");

	// Bogus operation value (DES) -> same as decrypting the plaintext
	memset(input_buffer, 0, sizeof(input_buffer));
	memcpy(input_buffer, "\x53\x65\x63\x72\x65\x74\x20\x73\x74\x72\x69\x6e\x67", 13);
	memset(output_buffer, 0, sizeof(output_buffer));
	memset(iv, 0xcc, sizeof(iv));
	XcBlockCryptCBC(cipher_selector, sizeof(input_buffer), output_buffer, input_buffer, key_table_des, 0xFFFFFFFF, iv);
	assert_hashed_result(output_buffer,
	                     sizeof(output_buffer),
	                     (PUCHAR)"\x1f\x28\xfc\x62\x17\xc2\x46\xa5\x10\x8c\xf1\xd8\xc5\x7a\xf6\x24\x30\xf6\x48\xf5",
	                     "bogus operation (DES)");

	// 3DES encrypt
	memset(input_buffer, 0, sizeof(input_buffer));
	memcpy(input_buffer, "\x53\x65\x63\x72\x65\x74\x20\x73\x74\x72\x69\x6e\x67", 13);
	memset(output_buffer, 0, sizeof(output_buffer));
	memset(iv, 0xcc, sizeof(iv));
	XcDESKeyParity(key_3des, sizeof(key_3des));
	cipher_selector = 1;
	XcKeyTable(cipher_selector, key_table_3des, key_3des);
	XcBlockCryptCBC(cipher_selector, sizeof(input_buffer), output_buffer, input_buffer, key_table_3des, 1, iv);
	assert_hashed_result(output_buffer,
	                     sizeof(output_buffer),
	                     (PUCHAR)"\x31\x34\xe5\xa6\x5f\x3b\x96\xef\x0e\x76\xe7\xea\xf4\xd3\x4e\x19\xf9\xc1\x1a\x1d",
	                     "3DES encrypt");

	// 3DES decrypt
	memcpy(input_buffer, output_buffer, sizeof(input_buffer));
	memset(output_buffer, 0, sizeof(output_buffer));
	memset(iv, 0xcc, sizeof(iv));
	XcBlockCryptCBC(cipher_selector, sizeof(input_buffer), output_buffer, input_buffer, key_table_3des, 0, iv);
	assert_hashed_result(output_buffer,
	                     sizeof(output_buffer),
	                     (PUCHAR)"\x8f\x11\x6d\xc7\x6a\xf3\xb1\x28\xa2\xca\x83\x0f\x1e\x1f\xe3\xc8\x4a\xe1\xe3\xce",
	                     "3DES decrypt");

	// 3DES decrypt plaintext
	memset(input_buffer, 0, sizeof(input_buffer));
	memcpy(input_buffer, "\x53\x65\x63\x72\x65\x74\x20\x73\x74\x72\x69\x6e\x67", 13);
	memset(output_buffer, 0, sizeof(output_buffer));
	memset(iv, 0xcc, sizeof(iv));
	XcBlockCryptCBC(cipher_selector, sizeof(input_buffer), output_buffer, input_buffer, key_table_3des, 0, iv);
	assert_hashed_result(output_buffer,
	                     sizeof(output_buffer),
	                     (PUCHAR)"\x42\xe5\x98\xc0\x66\x93\x1e\x81\xd0\xc4\xe2\x47\x00\x7a\x0c\x1f\x6b\x7a\x16\xbb",
	                     "3DES decrypt plaintext");

	// Bogus operation value (3DES) -> same as decrypting the plaintext
	memset(input_buffer, 0, sizeof(input_buffer));
	memcpy(input_buffer, "\x53\x65\x63\x72\x65\x74\x20\x73\x74\x72\x69\x6e\x67", 13);
	memset(output_buffer, 0, sizeof(output_buffer));
	memset(iv, 0xcc, sizeof(iv));
	XcBlockCryptCBC(cipher_selector, sizeof(input_buffer), output_buffer, input_buffer, key_table_3des, 0xFFFFFFFF, iv);
	assert_hashed_result(output_buffer,
	                     sizeof(output_buffer),
	                     (PUCHAR)"\x42\xe5\x98\xc0\x66\x93\x1e\x81\xd0\xc4\xe2\x47\x00\x7a\x0c\x1f\x6b\x7a\x16\xbb",
	                     "bogus operation (3DES)");

	// Bogus input length value (3DES encrypt) -> it encrypts up to des_block (length + 7) / 8
	memset(input_buffer, 0, sizeof(input_buffer));
	memcpy(input_buffer, "\x53\x65\x63\x72\x65\x74\x20\x73\x74\x72\x69\x6e\x67", 13);
	memset(output_buffer, 0, sizeof(output_buffer));
	memset(iv, 0xcc, sizeof(iv));
	XcBlockCryptCBC(cipher_selector, 11, output_buffer, input_buffer, key_table_3des, 1, iv);
	assert_hashed_result(output_buffer,
	                     sizeof(output_buffer),
	                     (PUCHAR)"\x4e\x9c\x6d\xf9\xcc\xac\xa8\xfe\x33\xba\x6a\x30\xb4\x17\x09\x0a\x9b\x3b\x82\x59",
	                     "bogus input length (3DES encrypt)");

	TEST_END();
}
