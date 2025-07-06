
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
        true, (CurrentAxisMicroSwitchState){0, 0, 0, 1}, TARGET_AXIS_MICROSWITCH_STATE_CENTER,
        "left -> center should be true", &failures
    );

    check_reverse(
        true, (CurrentAxisMicroSwitchState){0, 0, 0, 1}, TARGET_AXIS_MICROSWITCH_STATE_RIGHT,
        "left -> right should be true", &failures
    );

    check_reverse(
        false, (CurrentAxisMicroSwitchState){0, 1, 0, 0}, TARGET_AXIS_MICROSWITCH_STATE_LEFT,
        "center -> left should be false", &failures
    );

    check_reverse(
        true, (CurrentAxisMicroSwitchState){0, 1, 0, 0}, TARGET_AXIS_MICROSWITCH_STATE_RIGHT,
        "center -> right should be true", &failures
    );

    check_reverse(
        false, (CurrentAxisMicroSwitchState){0, 0, 0, 0}, TARGET_AXIS_MICROSWITCH_STATE_LEFT,
        "nothing -> left should be false", &failures
    );

    check_reverse(
        true, (CurrentAxisMicroSwitchState){0, 0, 0, 0}, TARGET_AXIS_MICROSWITCH_STATE_CENTER,
        "nothing -> center should be true", &failures
    );

    check_reverse(
        true, (CurrentAxisMicroSwitchState){0, 0, 0, 0}, TARGET_AXIS_MICROSWITCH_STATE_RIGHT,
        "nothing -> right should be true", &failures
    );

    check_reverse(
        false, (CurrentAxisMicroSwitchState){0, 0, 1, 0}, TARGET_AXIS_MICROSWITCH_STATE_CENTER,
        "right -> center should be false", &failures
    );

    check_reverse(
        false, (CurrentAxisMicroSwitchState){0, 0, 1, 0}, TARGET_AXIS_MICROSWITCH_STATE_LEFT,
        "right -> left should be false", &failures
    );

    check_reverse(
        false, (CurrentAxisMicroSwitchState){1, 0, 0, 0}, TARGET_AXIS_MICROSWITCH_STATE_LEFT,
        "left-center -> left should be false", &failures
    );

    check_reverse(
        false, (CurrentAxisMicroSwitchState){1, 0, 0, 0}, TARGET_AXIS_MICROSWITCH_STATE_CENTER,
        "left-center -> center should be false", &failures
    );

    check_reverse(
        true, (CurrentAxisMicroSwitchState){1, 0, 0, 0}, TARGET_AXIS_MICROSWITCH_STATE_RIGHT,
        "left-center -> right should be true", &failures
    );

    check_reverse(
        false, (CurrentAxisMicroSwitchState){1, 0, 1, 0}, TARGET_AXIS_MICROSWITCH_STATE_CENTER,
        "left-center + right -> center should be false", &failures
    );

    check_reverse(
        false, (CurrentAxisMicroSwitchState){1, 0, 1, 0}, TARGET_AXIS_MICROSWITCH_STATE_LEFT,
        "left-center + right -> left should be false", &failures
    );

    TEST_ASSERT_EQUAL_INT_MESSAGE(0, failures, "Some gearshift tests failed");
}