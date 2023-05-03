#include <xboxkrnl/xboxkrnl.h>
#include <string.h>

#include "util/output.h"
#include "assertions/xc.h"

void test_XcKeyTable()
{
	const char* func_num = "0x015b";
	const char* func_name = "XcKeyTable";
	BOOL test_passed = 1;
	ULONG cipher_selector;
	UCHAR key_table_des[128]    = { 0 };
	UCHAR key_table_3des[128*3] = { 0 };
	UCHAR key_des[8]            = { 0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17 };
	UCHAR key_3des[8*3]         = {
		0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,
		0x20,0x21,0x22,0x23,0x24,0x25,0x26,0x27,
		0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37
	};
	
	print_test_header(func_num, func_name);
	
	// DES
	XcDESKeyParity(key_des, sizeof(key_des));
	cipher_selector = 0;
	XcKeyTable(cipher_selector, key_table_des, key_des);
	test_passed &= assert_hashed_result(key_table_des,
	                                    sizeof(key_table_des),
	                                    (PUCHAR)"\xf4\x45\x75\x1a\xbd\xe0\x89\x3b\xd8\x6f\x5d\x02\xf3\x87\x83\x1f\x7d\xb8\x39\xc4",
	                                    "DES");
	
	// 3DES
	XcDESKeyParity(key_3des, sizeof(key_3des));
	cipher_selector = 1;
	XcKeyTable(cipher_selector, key_table_3des, key_3des);
	test_passed &= assert_hashed_result(key_table_3des,
	                                    sizeof(key_table_3des),
	                                    (PUCHAR)"\xc2\xcd\x96\x3a\x00\x63\x33\x82\xeb\x7c\x25\x78\x8e\x4e\x47\xa6\x52\x72\x1f\xec",
	                                    "3DES");
	
	// Bogus cipher selector value (3DES)
	memset(key_table_3des, 0, sizeof(key_table_3des));
	cipher_selector = 0xFFFFFFFF;
	XcKeyTable(cipher_selector, key_table_3des, key_3des);
	test_passed &= assert_hashed_result(key_table_3des,
	                                    sizeof(key_table_3des),
	                                    (PUCHAR)"\xc2\xcd\x96\x3a\x00\x63\x33\x82\xeb\x7c\x25\x78\x8e\x4e\x47\xa6\x52\x72\x1f\xec",
	                                    "bogus cipher selector (3DES)");
	
	print_test_footer(func_num, func_name, test_passed);
}
