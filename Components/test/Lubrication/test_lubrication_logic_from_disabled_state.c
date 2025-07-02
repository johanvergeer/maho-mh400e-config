#include "lubrication_logic.h"
#include "unity.h"

#include <stdbool.h>

void setUp(void) {}

void tearDown(void) {}

void test_to_remains_disabled_when_motion_is_disabled() {
    LubricationState state = {
        .state = LUBRICATION_STATE_DISABLED,
        .lubrication_start_time = 0.0f,
        .building_pressure_start_time = 0.0f
    };
    const LubricationSignals input = {.is_motion_enabled = false, .is_pressure_ok = false};

    const LubricationConfig config = {
        .is_enabled = true, .interval = 1, .pressure_timeout = 1.0f, .pressure_hold_time = 0
    };

    lubricate(.9f, input, &state, config);
    TEST_ASSERT_EQUAL(LUBRICATION_STATE_DISABLED, state.state);
}

void test_to_remains_disabled_when_disabled_in_config() {
    LubricationState state = {
        .state = LUBRICATION_STATE_DISABLED,
        .lubrication_start_time = 0.0f,
        .building_pressure_start_time = 0.0f
    };
    const LubricationSignals input = {.is_motion_enabled = true, .is_pressure_ok = false};

    const LubricationConfig config = {
        .is_enabled = false, .interval = 1, .pressure_timeout = 1.0f, .pressure_hold_time = 0
    };

    lubricate(.9f, input, &state, config);
    TEST_ASSERT_EQUAL(LUBRICATION_STATE_DISABLED, state.state);
}

void test_to_idle_when_previous_cycle_was_done_within_interval() {
    LubricationState state = {
        .state = LUBRICATION_STATE_DISABLED,
        .lubrication_start_time = 0.0f,
        .building_pressure_start_time = 0.0f
    };
    const LubricationSignals input = {.is_motion_enabled = true, .is_pressure_ok = false};

    const LubricationConfig config = {
        .is_enabled = true, .interval = 1, .pressure_timeout = 1.0f, .pressure_hold_time = 0
    };

    lubricate(.9f, input, &state, config);
    TEST_ASSERT_EQUAL(LUBRICATION_STATE_IDLE, state.state);
}

void test_to_building_pressure_when_previous_cycle_was_too_long_ago() {
    LubricationState state = {
        .state = LUBRICATION_STATE_DISABLED,
        .lubrication_start_time = 0.0f,
        .building_pressure_start_time = 0.0f
    };
    const LubricationSignals input = {.is_motion_enabled = true, .is_pressure_ok = false};

    const LubricationConfig config = {
        .is_enabled = true, .interval = 1, .pressure_timeout = 1.0f, .pressure_hold_time = 0
    };

    lubricate(1.1f, input, &state, config);
    TEST_ASSERT_EQUAL(LUBRICATION_STATE_BUILDING_PRESSURE, state.state);
}
