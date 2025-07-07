#include "gearbox_logic.h"
#include "unity.h"

void setUp(void) {}

void tearDown(void) {}

void test_enable_select_mid_position__target_not_center__return_false() {
    TEST_ASSERT_FALSE(enable_select_mid_position(TARGET_AXIS_MICROSWITCH_STATE_RIGHT));
    TEST_ASSERT_FALSE(enable_select_mid_position(TARGET_AXIS_MICROSWITCH_STATE_LEFT));
}

void test_enable_select_mid_position__target_is_center__return_true() {
    TEST_ASSERT_TRUE(enable_select_mid_position(TARGET_AXIS_MICROSWITCH_STATE_CENTER));
}
