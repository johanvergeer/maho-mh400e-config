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
    // Dit wordt automatisch na elke test aangeroepen (indien nodig)
}

static void check_lubrication_state(
    const bool isMotionEnabled,
    const LubricationState expectedState,
    const bool expectedPumpEnabled
) {
    const LubricationPumpInput input = {
        .isMotionEnabled = isMotionEnabled,
        .isPressureOk = false
    };

    const LubricationConfig config = {
        .isEnabled = true,
        .interval = 0,
        .pressureTimeout = 0,
        .pressureHoldTime = 0
    };

    LubricationPumpOutput output = lubricate(0.0f, input, config);

    TEST_ASSERT_EQUAL(expectedState, output.state);
    TEST_ASSERT_EQUAL(expectedPumpEnabled, output.isEnabled);
}

void test_lubrication_builds_pressure_when_both_enabled(void) {
    check_lubrication_state(true, BUILDING_PRESSURE, true);
}
