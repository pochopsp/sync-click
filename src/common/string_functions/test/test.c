#include "../../third_party/Unity-v2.5.2/unity.h"
#include "../string_functions.h"

static uint16_t res_uint16;
static uint8_t res_uint8;

void setUp(void) {
    res_uint16 = 433;
    res_uint8 = 67;
}

// --------------- tests for string_to_uint16 ---------------

void test_function_stringtouint16_should_return_true_0_with_0(void) {
	TEST_ASSERT_TRUE(string_to_uint16("0", &res_uint16));
    TEST_ASSERT_EQUAL_UINT16(0, res_uint16);
}

void test_function_stringtouint16_should_return_true_29900_with_29900(void) {
	TEST_ASSERT_TRUE(string_to_uint16("29900", &res_uint16));
    TEST_ASSERT_EQUAL_UINT16(29900, res_uint16);
}

void test_function_stringtouint16_should_return_false_433_with_minus1(void) {
	TEST_ASSERT_FALSE(string_to_uint16("-1", &res_uint16));
    TEST_ASSERT_EQUAL_UINT16(433, res_uint16);
}

void test_function_stringtouint16_should_return_true_65535_with_65535(void) {
	TEST_ASSERT_TRUE(string_to_uint16("65535", &res_uint16));
    TEST_ASSERT_EQUAL_UINT16(65535, res_uint16);
}

void test_function_stringtouint16_should_return_false_433_with_65536(void) {
	TEST_ASSERT_FALSE(string_to_uint16("65536", &res_uint16));
    TEST_ASSERT_EQUAL_UINT16(433, res_uint16);
}

// --------------- tests for string_to_uint8 ---------------

void test_function_stringtouint8_should_return_true_0_with_0(void) {
	TEST_ASSERT_TRUE(string_to_uint8("0", &res_uint8));
    TEST_ASSERT_EQUAL_UINT8(0, res_uint8);
}

void test_function_stringtouint8_should_return_true_99_with_99(void) {
	TEST_ASSERT_TRUE(string_to_uint8("99", &res_uint8));
    TEST_ASSERT_EQUAL_UINT8(99, res_uint8);
}

void test_function_stringtouint8_should_return_false_67_with_minus1(void) {
	TEST_ASSERT_FALSE(string_to_uint8("-1", &res_uint8));
    TEST_ASSERT_EQUAL_UINT8(67, res_uint8);
}

void test_function_stringtouint8_should_return_true_255_with_255(void) {
	TEST_ASSERT_TRUE(string_to_uint8("255", &res_uint8));
    TEST_ASSERT_EQUAL_UINT8(255, res_uint8);
}

void test_function_stringtouint8_should_return_false_67_with_256(void) {
	TEST_ASSERT_FALSE(string_to_uint8("256", &res_uint8));
    TEST_ASSERT_EQUAL_UINT8(67, res_uint8);
}