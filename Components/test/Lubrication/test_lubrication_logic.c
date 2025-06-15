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

void test_lubrication_remains_initializing_when_is_motion_enabled_and_config_is_enabled_are_both_false(void) {
    LubricationPumpInput input = {
        .isMotionEnabled = false,
        .isPressureOk = false
    };

    LubricationConfig config = {
        .isEnabled = false,
        .interval = 0,
        .pressureTimeout = 0,
        .pressureHoldTime = 0
    };

    LubricationPumpOutput output = lubricate(0.0f, input, config);

    TEST_ASSERT_EQUAL(INITIALIZING, output.state);
    TEST_ASSERT_FALSE(output.isEnabled);
}

void test_lubrication_remains_initializing_when_is_motion_enabled_is_false(void) {
    LubricationPumpInput input = {
        .isMotionEnabled = false,
        .isPressureOk = false
    };

    LubricationConfig config = {
        .isEnabled = true,
        .interval = 0,
        .pressureTimeout = 0,
        .pressureHoldTime = 0
    };

    LubricationPumpOutput output = lubricate(0.0f, input, config);

    TEST_ASSERT_EQUAL(INITIALIZING, output.state);
    TEST_ASSERT_FALSE(output.isEnabled);
}

void test_lubrication_remains_initializing_when_is_config_is_enabled_is_false(void) {
    LubricationPumpInput input = {
        .isMotionEnabled = true,
        .isPressureOk = false
    };

    LubricationConfig config = {
        .isEnabled = false,
        .interval = 0,
        .pressureTimeout = 0,
        .pressureHoldTime = 0
    };

    LubricationPumpOutput output = lubricate(0.0f, input, config);

    TEST_ASSERT_EQUAL(INITIALIZING, output.state);
    TEST_ASSERT_FALSE(output.isEnabled);
}

void test_lubrication_is_pressurizing_when_motion_is_enabled_and_config_is_enabled(void) {
    LubricationPumpInput input = {
        .isMotionEnabled = true,
        .isPressureOk = false
    };

    LubricationConfig config = {
        .isEnabled = true,
        .interval = 0,
        .pressureTimeout = 0,
        .pressureHoldTime = 0
    };

    LubricationPumpOutput output = lubricate(0.0f, input, config);

    TEST_ASSERT_EQUAL(BUILDING_PRESSURE, output.state);
    TEST_ASSERT_TRUE(output.isEnabled);
}