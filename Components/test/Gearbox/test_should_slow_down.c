
#include "gearbox_logic.h"
#include "unity.h"

void setUp(void) {}

void tearDown(void) {}

void test_should_slow_down__center_micro_switch_not_enabled__return_false() {
    CurrentAxisMicroSwitchState current_state =
        (CurrentAxisMicroSwitchState){false, false, false, false};
    TargetAxisMicroSwitchState target_state = (TargetAxisMicroSwitchState){true, false, false};

    TEST_ASSERT_FALSE(should_slow_down(current_state, target_state));
}

void test_should_slow_down__target_not_center__return_false() {
    CurrentAxisMicroSwitchState current_state =
        (CurrentAxisMicroSwitchState){false, true, false, false};
    TargetAxisMicroSwitchState target_state = (TargetAxisMicroSwitchState){false, true, false};

    TEST_ASSERT_FALSE(should_slow_down(current_state, target_state));
}

void test_should_slow_down__target_is_center_and_center_micro_switch_enabled__return_true() {
    CurrentAxisMicroSwitchState current_state =
        (CurrentAxisMicroSwitchState){false, true, false, false};
    TargetAxisMicroSwitchState target_state = (TargetAxisMicroSwitchState){true, false, false};

    TEST_ASSERT_TRUE(should_slow_down(current_state, target_state));
}