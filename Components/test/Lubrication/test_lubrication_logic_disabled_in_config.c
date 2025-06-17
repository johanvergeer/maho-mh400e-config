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

void test_lubrication_logic_disabled(void) {
    const LubricationPumpInput input = {
        .isMotionEnabled = true,
        .isPressureOk = false
    };

    const LubricationConfig config = {
        .isEnabled = false,
        .interval = 0,
        .pressureTimeout = 0,
        .pressureHoldTime = 0
    };
    TEST_ASSERT_EQUAL(DISABLED, lubricate( 0.0f, input, config ).state);
}
