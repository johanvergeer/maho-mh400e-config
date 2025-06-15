//
// Created by Johan Vergeer on 15/06/2025.
//
#include "unity.h"
#include "calculate_logic.h"

void setUp(void) {
    // Dit wordt automatisch voor elke test aangeroepen (indien nodig)
}

void tearDown(void) {
    // Dit wordt automatisch na elke test aangeroepen (indien nodig)
}

void test_calculate_logic_return_correct_value(void) {
    TEST_ASSERT_EQUAL(3, calculate(1));
}