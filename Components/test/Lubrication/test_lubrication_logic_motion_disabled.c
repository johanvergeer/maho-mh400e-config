#include "lubrication_logic.h"
#include "unity.h"

#include <stdbool.h>

void setUp(void) {}

void tearDown(void) {}

void test_lubrication_logic_motion_disabled(void) {
    LubricationState state = {
        .state = LUBRICATION_STATE_BUILDING_PRESSURE,
        .lubrication_start_time = 0.0f,
        .building_pressure_start_time = 0.0f
    };
    const LubricationSignals input = {.is_motion_enabled = false, .is_pressure_ok = false};

    const LubricationConfig config = {
        .is_enabled = true, .interval = 0, .pressure_timeout = 0, .pressure_hold_time = 0
    };
    lubricate(0.0f, input, &state, config);
    TEST_ASSERT_EQUAL(LUBRICATION_STATE_DISABLED, state.state);
}
