#include "../../../common/third_party/Unity-v2.5.2/unity.h"
#include "../network_functions.h"

void test_function_getinterfaceip_should_return_true_127001_with_lo(void) {
	char ip_buffer[16];
	TEST_ASSERT_TRUE(get_interface_ip("lo", ip_buffer));
    TEST_ASSERT_EQUAL_STRING("127.0.0.1", ip_buffer);
}

void test_function_getinterfaceip_should_return_false_empty_with_fakeinterface(void) {
	char ip_buffer[16] = {'\0'};
	TEST_ASSERT_FALSE(get_interface_ip("fakeinterface", ip_buffer));
    TEST_ASSERT_EQUAL_STRING("", ip_buffer);
}