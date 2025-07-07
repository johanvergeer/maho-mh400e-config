#include "gearbox_logic.h"
#include "unity.h"

void setUp(void) {}

void tearDown(void) {}

static void check_target_state(
    const float rpm,
    const TargetGearboxMicroSwitchesState expected,
    const char *message,
    int *failures
) {
    TargetGearboxMicroSwitchesState actual = get_target_state(rpm);
    if (actual.input != expected.input || actual.middle != expected.middle ||
        actual.reducer != expected.reducer) {
        printf("FAIL: %s (rpm=%.1f)\n", message, rpm);
        printf(
            "       got:     input=%d middle=%d reducer=%d\n", actual.input, actual.middle,
            actual.reducer
        );
        printf(
            "       expected: input=%d middle=%d reducer=%d\n", expected.input, expected.middle,
            expected.reducer
        );
        (*failures)++;
    }
}

void test_get_target_state(void) {
    int failures = 0;

    check_target_state(
        0.0f,
        (TargetGearboxMicroSwitchesState){TARGET_AXIS_MICROSWITCH_STATE_CENTER,
                                          TARGET_AXIS_MICROSWITCH_STATE_CENTER,
                                          TARGET_AXIS_MICROSWITCH_STATE_CENTER},
        "Neutral (0 rpm)", &failures
    );

    check_target_state(
        50.0f,
        (TargetGearboxMicroSwitchesState){TARGET_AXIS_MICROSWITCH_STATE_LEFT,
                                          TARGET_AXIS_MICROSWITCH_STATE_CENTER,
                                          TARGET_AXIS_MICROSWITCH_STATE_LEFT},
        "Low RPM falls in 80 rpm zone", &failures
    );

    check_target_state(
        125.0f,
        (TargetGearboxMicroSwitchesState){TARGET_AXIS_MICROSWITCH_STATE_RIGHT,
                                          TARGET_AXIS_MICROSWITCH_STATE_CENTER,
                                          TARGET_AXIS_MICROSWITCH_STATE_LEFT},
        "Midrange 125 rpm zone", &failures
    );

    check_target_state(
        500.0f,
        (TargetGearboxMicroSwitchesState){TARGET_AXIS_MICROSWITCH_STATE_LEFT,
                                          TARGET_AXIS_MICROSWITCH_STATE_CENTER,
                                          TARGET_AXIS_MICROSWITCH_STATE_RIGHT},
        "Exactly 500 rpm", &failures
    );

    check_target_state(
        1000.0f,
        (TargetGearboxMicroSwitchesState){TARGET_AXIS_MICROSWITCH_STATE_CENTER,
                                          TARGET_AXIS_MICROSWITCH_STATE_RIGHT,
                                          TARGET_AXIS_MICROSWITCH_STATE_CENTER},
        "Exactly 1000 rpm", &failures
    );

    check_target_state(
        3574.9f,
        (TargetGearboxMicroSwitchesState){TARGET_AXIS_MICROSWITCH_STATE_RIGHT,
                                          TARGET_AXIS_MICROSWITCH_STATE_CENTER,
                                          TARGET_AXIS_MICROSWITCH_STATE_LEFT},
        "Just below max mapping", &failures
    );

    check_target_state(
        3575.0f,
        (TargetGearboxMicroSwitchesState){TARGET_AXIS_MICROSWITCH_STATE_RIGHT,
                                          TARGET_AXIS_MICROSWITCH_STATE_RIGHT,
                                          TARGET_AXIS_MICROSWITCH_STATE_RIGHT},
        "Exactly at fallback threshold", &failures
    );

    check_target_state(
        10000.0f,
        (TargetGearboxMicroSwitchesState){TARGET_AXIS_MICROSWITCH_STATE_RIGHT,
                                          TARGET_AXIS_MICROSWITCH_STATE_RIGHT,
                                          TARGET_AXIS_MICROSWITCH_STATE_RIGHT},
        "Above fallback threshold", &failures
    );

    TEST_ASSERT_EQUAL_INT_MESSAGE(0, failures, "Some gearbox target state tests failed");
}
