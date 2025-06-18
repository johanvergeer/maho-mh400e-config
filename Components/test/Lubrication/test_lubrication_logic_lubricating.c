//
// Created by Johan Vergeer on 15/06/2025.
//
#include "unity.h"
#include "lubrication_logic.h"
#include <stdbool.h>


void setUp(void) {
}

void tearDown(void) {
}

void test_keeps_lubricating_when_pressure_hold_time_has_not_expired(void) {
    LubricationState state = {
        .state = LUBRICATION_STATE_LUBRICATING,
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
        .pressureTimeout = 0,
        .pressureHoldTime = 1
    };

    lubricate(.9f, input, &state, config);

    TEST_ASSERT_EQUAL(LUBRICATION_STATE_LUBRICATING, state.state);
}

void test_lubrication_stops_when_pressure_hold_time_has_expired(void) {
    LubricationState state = {
        .state = LUBRICATION_STATE_LUBRICATING,
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
        .pressureTimeout = 0,
        .pressureHoldTime = 1
    };

    lubricate(1.1f, input, &state, config);

    TEST_ASSERT_EQUAL(LUBRICATION_STATE_IDLE, state.state);
}
