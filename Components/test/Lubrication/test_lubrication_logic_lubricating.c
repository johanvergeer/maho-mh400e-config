#include "lubrication_logic.h"
#include "unity.h"

#include <stdbool.h>

void setUp(void) {}

void tearDown(void) {}

void test_keeps_lubricating_when_pressure_hold_time_has_not_expired(void) {
    LubricationState state = {
        .state = LUBRICATION_STATE_LUBRICATING,
        .lubrication_start_time = 0.0f,
        .building_pressure_start_time = 0.0f
    };
    const LubricationSignals input = {.is_motion_enabled = true, .is_pressure_ok = false};

    const LubricationConfig config = {
        .is_enabled = true, .interval = 0, .pressure_timeout = 0, .pressure_hold_time = 1
    };

    lubricate(.9f, input, &state, config);

    TEST_ASSERT_EQUAL(LUBRICATION_STATE_LUBRICATING, state.state);
}

void test_lubrication_stops_when_pressure_hold_time_has_expired(void) {
    LubricationState state = {
        .state = LUBRICATION_STATE_LUBRICATING,
        .lubrication_start_time = 0.0f,
        .building_pressure_start_time = 0.0f
    };
    const LubricationSignals input = {.is_motion_enabled = true, .is_pressure_ok = false};

    const LubricationConfig config = {
        .is_enabled = true, .interval = 0, .pressure_timeout = 0, .pressure_hold_time = 1
    };

    lubricate(1.1f, input, &state, config);

    TEST_ASSERT_EQUAL(LUBRICATION_STATE_IDLE, state.state);
}
