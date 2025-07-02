
#include "gearbox_logic.h"
#include "unity.h"

void setUp(void) {}

void tearDown(void) {}

static void check_reverse(
    const bool expected,
    const CurrentAxisMicroSwitchState from,
    const TargetAxisMicroSwitchState to,
    const char *message,
    int *failures
) {
    const bool result = gearshift_needs_reverse(from, to);
    if (result != expected) {
        printf(
            "FAIL: %s (got %s, expected %s)\n", message, result ? "true" : "false",
            expected ? "true" : "false"
        );
        (*failures)++;
    }
}

void test_gearbox_needs_reverse(void) {
    int failures = 0;

    check_reverse(
        true, (CurrentAxisMicroSwitchState){0, 0, 0, 1}, (TargetAxisMicroSwitchState){1, 0, 0},
        "left -> center should be true", &failures
    );

    check_reverse(
        true, (CurrentAxisMicroSwitchState){0, 0, 0, 1}, (TargetAxisMicroSwitchState){0, 1, 0},
        "left -> right should be true", &failures
    );

    check_reverse(
        false, (CurrentAxisMicroSwitchState){0, 1, 0, 0}, (TargetAxisMicroSwitchState){0, 0, 1},
        "center -> left should be false", &failures
    );

    check_reverse(
        true, (CurrentAxisMicroSwitchState){0, 1, 0, 0}, (TargetAxisMicroSwitchState){0, 1, 0},
        "center -> right should be true", &failures
    );

    check_reverse(
        false, (CurrentAxisMicroSwitchState){0, 0, 0, 0}, (TargetAxisMicroSwitchState){0, 0, 1},
        "nothing -> left should be false", &failures
    );

    check_reverse(
        true, (CurrentAxisMicroSwitchState){0, 0, 0, 0}, (TargetAxisMicroSwitchState){1, 0, 0},
        "nothing -> center should be true", &failures
    );

    check_reverse(
        true, (CurrentAxisMicroSwitchState){0, 0, 0, 0}, (TargetAxisMicroSwitchState){0, 1, 0},
        "nothing -> right should be true", &failures
    );

    check_reverse(
        false, (CurrentAxisMicroSwitchState){0, 0, 1, 0}, (TargetAxisMicroSwitchState){1, 0, 0},
        "right -> center should be false", &failures
    );

    check_reverse(
        false, (CurrentAxisMicroSwitchState){0, 0, 1, 0}, (TargetAxisMicroSwitchState){0, 0, 1},
        "right -> left should be false", &failures
    );

    check_reverse(
        false, (CurrentAxisMicroSwitchState){1, 0, 0, 0}, (TargetAxisMicroSwitchState){0, 0, 1},
        "left-center -> left should be false", &failures
    );

    check_reverse(
        false, (CurrentAxisMicroSwitchState){1, 0, 0, 0}, (TargetAxisMicroSwitchState){1, 0, 0},
        "left-center -> center should be false", &failures
    );

    check_reverse(
        true, (CurrentAxisMicroSwitchState){1, 0, 0, 0}, (TargetAxisMicroSwitchState){0, 1, 0},
        "left-center -> right should be true", &failures
    );

    check_reverse(
        false, (CurrentAxisMicroSwitchState){1, 0, 1, 0}, (TargetAxisMicroSwitchState){1, 0, 0},
        "left-center + right -> center should be false", &failures
    );

    check_reverse(
        false, (CurrentAxisMicroSwitchState){1, 0, 1, 0}, (TargetAxisMicroSwitchState){0, 0, 1},
        "left-center + right -> left should be false", &failures
    );

    TEST_ASSERT_EQUAL_INT_MESSAGE(0, failures, "Some gearshift tests failed");
}