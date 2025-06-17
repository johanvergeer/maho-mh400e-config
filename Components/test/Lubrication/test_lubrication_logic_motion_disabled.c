//
// Created by Johan Vergeer on 17/06/2025.
//
#include "unity.h"
#include "lubrication_logic.h"
#include <stdbool.h>


void setUp(void) {
}

void tearDown(void) {
}

void test_lubrication_logic_motion_disabled(void) {
    LubricationState state = {
        .state = BUILDING_PRESSURE,
        .lubricationStartTime = 0.0f,
        .buildingPressureStartTime = 0.0f
    };
    const LubricationSignals input = {
        .isMotionEnabled = false,
        .isPressureOk = false
    };

    const LubricationConfig config = {
        .isEnabled = true,
        .interval = 0,
        .pressureTimeout = 0,
        .pressureHoldTime = 0
    };
    lubricate(0.0f, input, &state, config);
    TEST_ASSERT_EQUAL(DISABLED, state.state);
}
