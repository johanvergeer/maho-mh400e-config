#include "unity.h"
#include "lubrication_logic.h"
#include <stdbool.h>


void setUp(void) {
}

void tearDown(void) {
}

void test_building_pressure_state_does_not_change_when_pressure_is_not_ok() {
    LubricationState state = {
        .state = LUBRICATION_STATE_BUILDING_PRESSURE,
        .lubricationStartTime = 0.0f,
        .buildingPressureStartTime = 0.0f
    };
    const LubricationSignals input = {
        .isMotionEnabled = true,
        .isPressureOk = false
    };

    const LubricationConfig config = {
        .isEnabled = true,
        .interval = 0,
        .pressureTimeout = 1.0f,
        .pressureHoldTime = 0
    };

    // State should still be BUILDING_PRESSURE_WHEN_PRESSURE_IS_NOT_REACHED
    lubricate(.9f, input, &state, config);
    TEST_ASSERT_EQUAL(LUBRICATION_STATE_BUILDING_PRESSURE, state.state);
}

void test_building_pressure_changes_to_lubricating_when_pressure_is_ok() {
    LubricationState state = {
        .state = LUBRICATION_STATE_BUILDING_PRESSURE,
        .lubricationStartTime = 0.0f,
        .buildingPressureStartTime = 0.0f
    };
    const LubricationConfig config = {
        .isEnabled = true,
        .interval = 0,
        .pressureTimeout = 0,
        .pressureHoldTime = 0
    };

    const LubricationSignals input = {
        .isMotionEnabled = true,
        .isPressureOk = true
    };

    lubricate(1.0f, input, &state, config);

    // State should switch to LUBRICATING when pressure is ok
    TEST_ASSERT_EQUAL(LUBRICATION_STATE_LUBRICATING, state.state);
    TEST_ASSERT_EQUAL(1.0f, state.lubricationStartTime);
}

void test_building_pressure_changes_to_error_when_pressure_timeout_reached() {
    LubricationState state = {
        .state = LUBRICATION_STATE_BUILDING_PRESSURE,
        .lubricationStartTime = 0.0f,
        .buildingPressureStartTime = 0.0f
    };
    const LubricationConfig config = {
        .isEnabled = true,
        .interval = 0,
        .pressureTimeout = 1.0f,
        .pressureHoldTime = 0
    };

    const LubricationSignals input = {
        .isMotionEnabled = true,
        .isPressureOk = false
    };

    // State should switch to ERROR when building pressure has timed out
    lubricate(1.1f, input, &state, config);
    TEST_ASSERT_EQUAL(LUBRICATION_STATE_ERROR, state.state);
}

void test_building_pressure_remains_when_pressure_not_reached_yet() {
    LubricationState state = {
        .state = LUBRICATION_STATE_BUILDING_PRESSURE,
        .lubricationStartTime = 0.0f,
        .buildingPressureStartTime = 0.0f
    };
    const LubricationConfig config = {
        .isEnabled = true,
        .interval = 0,
        .pressureTimeout = 1.0f,
        .pressureHoldTime = 0
    };

    const LubricationSignals input = {
        .isMotionEnabled = true,
        .isPressureOk = false
    };

    lubricate(1.0f, input, &state, config);
    // State should switch to ERROR when building pressure has timed out
    TEST_ASSERT_EQUAL(LUBRICATION_STATE_BUILDING_PRESSURE, state.state);
}
