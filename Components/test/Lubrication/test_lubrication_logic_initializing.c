//
// Created by Johan Vergeer on 15/06/2025.
//
#include "unity.h"
#include "lubrication_logic.h"
#include <stdbool.h>


void setUp(void) {
    // Dit wordt automatisch voor elke test aangeroepen (indien nodig)
}

void tearDown(void) {
    // Dit wordt automatisch na elke test aangeroepen (indien nodig)
}

static void check_lubrication_state(
    bool isMotionEnabled,
    bool isEnabled,
    LubricationState expectedState,
    bool expectedPumpEnabled
) {
    LubricationPumpInput input = {
        .isMotionEnabled = isMotionEnabled,
        .isPressureOk = false
    };

    LubricationConfig config = {
        .isEnabled = isEnabled,
        .interval = 0,
        .pressureTimeout = 0,
        .pressureHoldTime = 0
    };

    LubricationPumpOutput output = lubricate(0.0f, input, config);

    TEST_ASSERT_EQUAL(expectedState, output.state);
    TEST_ASSERT_EQUAL(expectedPumpEnabled, output.isEnabled);
}

void test_lubrication_remains_initializing_when_both_disabled(void) {
    check_lubrication_state(false, false, INITIALIZING, false);
}

void test_lubrication_remains_initializing_when_motion_disabled(void) {
    check_lubrication_state(false, true, INITIALIZING, false);
}

void test_lubrication_remains_initializing_when_config_disabled(void) {
    check_lubrication_state(true, false, INITIALIZING, false);
}

void test_lubrication_builds_pressure_when_both_enabled(void) {
    check_lubrication_state(true, true, BUILDING_PRESSURE, true);
}
