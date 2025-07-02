#include "unity.h"
#include "supported_speeds.h"
#include <stdbool.h>


void setUp(void) {
}

void tearDown(void) {
}

void test_supported_speeds_count(void) {
    TEST_ASSERT_EQUAL(19, SUPPORTED_SPEEDS_COUNT);
}

void test_get_rpm_from_bitmask_returns_0_when_rpm_not_found(void) {
    TEST_ASSERT_EQUAL(0, get_rpm_from_bitmask(1));
}

void test_get_rpm_from_bitmask_returns_rpm_when_found(void) {
    TEST_ASSERT_EQUAL(80, get_rpm_from_bitmask(1097));
}

void test_get_bitmask_from_rpm_returns_0_when_bitmask_not_found(void) {
    TEST_ASSERT_EQUAL(0, get_bitmask_from_rpm(1));
}

void test_get_bitmask_from_rpm_returns_bitmask_when_found(void) {
    TEST_ASSERT_EQUAL(1097, get_bitmask_from_rpm(80));
}
