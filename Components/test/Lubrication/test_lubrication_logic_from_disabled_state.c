#include "unity.h"
#include "lubrication_logic.h"
#include <stdbool.h>


void setUp(void) {
}

void tearDown(void) {
}


void test_to_remains_disabled_when_motion_is_disabled() {
    LubricationState state = {
        .state = LUBRICATION_STATE_DISABLED,
        .lubricationStartTime = 0.0f,
        .buildingPressureStartTime = 0.0f
    };
    const LubricationSignals input = {
        .isMotionEnabled = false,
        .isPressureOk = false
    };

    const LubricationConfig config = {
        .isEnabled = true,
        .interval = 1,
        .pressureTimeout = 1.0f,
        .pressureHoldTime = 0
    };

    lubricate(.9f, input, &state, config);
    TEST_ASSERT_EQUAL(LUBRICATION_STATE_DISABLED, state.state);
}


void test_to_remains_disabled_when_disabled_in_config() {
    LubricationState state = {
        .state = LUBRICATION_STATE_DISABLED,
        .lubricationStartTime = 0.0f,
        .buildingPressureStartTime = 0.0f
    };
    const LubricationSignals input = {
        .isMotionEnabled = true,
        .isPressureOk = false
    };

    const LubricationConfig config = {
        .isEnabled = false,
        .interval = 1,
        .pressureTimeout = 1.0f,
        .pressureHoldTime = 0
    };

    lubricate(.9f, input, &state, config);
    TEST_ASSERT_EQUAL(LUBRICATION_STATE_DISABLED, state.state);
}

void test_to_idle_when_previous_cycle_was_done_within_interval() {
    LubricationState state = {
        .state = LUBRICATION_STATE_DISABLED,
        .lubricationStartTime = 0.0f,
        .buildingPressureStartTime = 0.0f
    };
    const LubricationSignals input = {
        .isMotionEnabled = true,
        .isPressureOk = false
    };

    const LubricationConfig config = {
        .isEnabled = true,
        .interval = 1,
        .pressureTimeout = 1.0f,
        .pressureHoldTime = 0
    };

    lubricate(.9f, input, &state, config);
    TEST_ASSERT_EQUAL(LUBRICATION_STATE_IDLE, state.state);
}


void test_to_building_pressure_when_previous_cycle_was_too_long_ago() {
    LubricationState state = {
        .state = LUBRICATION_STATE_DISABLED,
        .lubricationStartTime = 0.0f,
        .buildingPressureStartTime = 0.0f
    };
    const LubricationSignals input = {
        .isMotionEnabled = true,
        .isPressureOk = false
    };

    const LubricationConfig config = {
        .isEnabled = true,
        .interval = 1,
        .pressureTimeout = 1.0f,
        .pressureHoldTime = 0
    };

    lubricate(1.1f, input, &state, config);
    TEST_ASSERT_EQUAL(LUBRICATION_STATE_BUILDING_PRESSURE, state.state);
}
