#include "lubrication_logic.h"
#include "unity.h"

#include <stdbool.h>

void setUp(void) {}

void tearDown(void) {}

void test_building_pressure_state_does_not_change_when_pressure_is_not_ok() {
    LubricationState state = {
        .state = LUBRICATION_STATE_BUILDING_PRESSURE,
        .lubrication_start_time = 0.0f,
        .building_pressure_start_time = 0.0f
    };
    const LubricationSignals input = {.is_motion_enabled = true, .is_pressure_ok = false};

    const LubricationConfig config = {
        .is_enabled = true, .interval = 0, .pressure_timeout = 1.0f, .pressure_hold_time = 0
    };

    // State should still be BUILDING_PRESSURE_WHEN_PRESSURE_IS_NOT_REACHED
    lubricate(.9f, input, &state, config);
    TEST_ASSERT_EQUAL(LUBRICATION_STATE_BUILDING_PRESSURE, state.state);
}

void test_building_pressure_changes_to_lubricating_when_pressure_is_ok() {
    LubricationState state = {
        .state = LUBRICATION_STATE_BUILDING_PRESSURE,
        .lubrication_start_time = 0.0f,
        .building_pressure_start_time = 0.0f
    };
    const LubricationConfig config = {
        .is_enabled = true, .interval = 0, .pressure_timeout = 0, .pressure_hold_time = 0
    };

    const LubricationSignals input = {.is_motion_enabled = true, .is_pressure_ok = true};

    lubricate(1.0f, input, &state, config);

    // State should switch to LUBRICATING when pressure is ok
    TEST_ASSERT_EQUAL(LUBRICATION_STATE_LUBRICATING, state.state);
    TEST_ASSERT_EQUAL(1.0f, state.lubrication_start_time);
}

void test_building_pressure_changes_to_error_when_pressure_timeout_reached() {
    LubricationState state = {
        .state = LUBRICATION_STATE_BUILDING_PRESSURE,
        .lubrication_start_time = 0.0f,
        .building_pressure_start_time = 0.0f
    };
    const LubricationConfig config = {
        .is_enabled = true, .interval = 0, .pressure_timeout = 1.0f, .pressure_hold_time = 0
    };

    const LubricationSignals input = {.is_motion_enabled = true, .is_pressure_ok = false};

    // State should switch to ERROR when building pressure has timed out
    lubricate(1.1f, input, &state, config);
    TEST_ASSERT_EQUAL(LUBRICATION_STATE_ERROR, state.state);
}

void test_building_pressure_remains_when_pressure_not_reached_yet() {
    LubricationState state = {
        .state = LUBRICATION_STATE_BUILDING_PRESSURE,
        .lubrication_start_time = 0.0f,
        .building_pressure_start_time = 0.0f
    };
    const LubricationConfig config = {
        .is_enabled = true, .interval = 0, .pressure_timeout = 1.0f, .pressure_hold_time = 0
    };

    const LubricationSignals input = {.is_motion_enabled = true, .is_pressure_ok = false};

    lubricate(1.0f, input, &state, config);
    // State should switch to ERROR when building pressure has timed out
    TEST_ASSERT_EQUAL(LUBRICATION_STATE_BUILDING_PRESSURE, state.state);
}
