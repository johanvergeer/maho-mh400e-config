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

void test_building_pressure_state_does_not_change_when_pressure_is_not_ok() {
    const LubricationPumpInput input = {
        .isMotionEnabled = true,
        .isPressureOk = false
    };

    const LubricationConfig config = {
        .isEnabled = true,
        .interval = 0,
        .pressureTimeout = 1.0f,
        .pressureHoldTime = 0
    };

    // First call lubricate to switch state to BUILDING_PRESSURE
    TEST_ASSERT_EQUAL(BUILDING_PRESSURE, lubricate( 0.0f, input, config ).state);

    // State should still be BUILDING_PRESSURE_WHEN_PRESSURE_IS_NOT_REACHED
    TEST_ASSERT_EQUAL(BUILDING_PRESSURE, lubricate( .9f, input, config ).state);
}

void test_building_pressure_changes_to_lubricating_when_pressure_is_ok() {
    const LubricationConfig config = {
        .isEnabled = true,
        .interval = 0,
        .pressureTimeout = 0,
        .pressureHoldTime = 0
    };

    const LubricationPumpInput input1 = {
        .isMotionEnabled = true,
        .isPressureOk = false
    };

    // First call lubricate to switch state to BUILDING_PRESSURE
    TEST_ASSERT_EQUAL(BUILDING_PRESSURE, lubricate( 0.0f, input1, config ).state);

    const LubricationPumpInput input2 = {
        .isMotionEnabled = true,
        .isPressureOk = true
    };

    // State should switch to LUBRICATING when pressure is ok
    TEST_ASSERT_EQUAL(LUBRICATING, lubricate( 1.0f, input2, config ).state);
}

void test_building_pressure_changes_to_error_when_pressure_timeout_reached() {
    const LubricationConfig config = {
        .isEnabled = true,
        .interval = 0,
        .pressureTimeout = 1.0f,
        .pressureHoldTime = 0
    };

    const LubricationPumpInput input = {
        .isMotionEnabled = true,
        .isPressureOk = false
    };

    // First call lubricate to switch state to BUILDING_PRESSURE
    TEST_ASSERT_EQUAL(BUILDING_PRESSURE, lubricate( 0.1f, input, config ).state);

    // State should switch to ERROR when building pressure has timed out
    TEST_ASSERT_EQUAL(ERROR, lubricate( 1.2f, input, config ).state);
}
