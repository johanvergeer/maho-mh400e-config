//
// Created by Johan Vergeer on 15/06/2025.
//
#include "unity.h"
#include "lubrication_logic.h"
#include <stdbool.h>


void setUp(void) {
    lubrication_reset();
}

void tearDown(void) {
}

void test_lubrication_builds_pressure_when_both_enabled(void) {
    const LubricationPumpInput input = {
        .isMotionEnabled = true,
        .isPressureOk = false
    };

    const LubricationConfig config = {
        .isEnabled = true,
        .interval = 0,
        .pressureTimeout = 0,
        .pressureHoldTime = 0
    };

    LubricationPumpOutput output = lubricate(0.0f, input, config);

    TEST_ASSERT_EQUAL(BUILDING_PRESSURE, output.state);
}
