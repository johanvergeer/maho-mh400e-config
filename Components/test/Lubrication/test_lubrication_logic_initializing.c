#include "lubrication_logic.h"
#include "unity.h"

#include <stdbool.h>

void setUp(void) {}

void tearDown(void) {}

void test_lubrication_builds_pressure_when_both_enabled(void) {
    LubricationState state = {
        .state = LUBRICATION_STATE_INITIALIZING,
        .lubrication_start_time = 0.0f,
        .building_pressure_start_time = 0.0f
    };
    const LubricationSignals input = {.is_motion_enabled = true, .is_pressure_ok = false};

    const LubricationConfig config = {
        .is_enabled = true, .interval = 0, .pressure_timeout = 0, .pressure_hold_time = 0
    };

    lubricate(0.0f, input, &state, config);

    TEST_ASSERT_EQUAL(LUBRICATION_STATE_BUILDING_PRESSURE, state.state);
}
