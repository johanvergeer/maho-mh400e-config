#include "gearbox_logic.h"

SupportedSpeed supported_speeds[] = {
    /* rpm   bitmask                msb 11 10 9 8 7 6 5 4 3 2 1 0 lsb */
    {0, 4},       /* neutral           0 1 0 0 */
    {80, 1097},   /*   0 1 0 0 0 1 0 0 1 0 0 1 */
    {100, 2377},  /*   1 0 0 1 0 1 0 0 1 0 0 1 */
    {125, 585},   /*   1 0 1 0 0 1 0 0 1 0 0 1 */
    {160, 1177},  /*   0 1 0 0 1 0 0 1 1 0 0 1 */
    {200, 2457},  /*   1 0 0 1 1 0 0 1 1 0 0 1 */
    {250, 665},   /*   0 0 1 0 1 0 0 1 1 0 0 1 */
    {315, 1065},  /*   0 1 0 0 0 0 1 0 1 0 0 1 */
    {400, 2345},  /*   1 0 0 1 0 0 1 0 1 0 0 1 */
    {500, 553},   /*   0 0 1 0 0 0 1 0 1 0 0 1 */
    {630, 1090},  /*   0 1 0 0 0 1 0 0 0 0 1 0 */
    {800, 2370},  /*   1 0 0 1 0 1 0 0 0 0 1 0 */
    {1000, 578},  /*   0 0 1 0 0 1 0 0 0 0 1 0 */
    {1250, 1170}, /*   0 1 0 0 1 0 0 1 0 0 1 0 */
    {1600, 2450}, /*   1 0 0 1 1 0 0 1 0 0 1 0 */
    {2000, 658},  /*   0 0 1 0 1 0 0 1 0 0 1 0 */
    {2500, 1058}, /*   0 1 0 0 0 0 1 0 0 0 1 0 */
    {3150, 2338}, /*   1 0 0 1 0 0 1 0 0 0 1 0 */
    {4000, 546}   /*   0 0 1 0 0 0 1 0 0 0 1 0 */
};

const size_t SUPPORTED_SPEEDS_COUNT = sizeof(supported_speeds) / sizeof(supported_speeds[0]);

unsigned get_bitmask_from_rpm(const unsigned rpm) {
    for (size_t i = 0; i < SUPPORTED_SPEEDS_COUNT; ++i) {
        if (supported_speeds[i].rpm == rpm) {
            return supported_speeds[i].bitmask;
        }
    }
    return 0;
}

unsigned get_rpm_from_bitmask(const unsigned bitmask) {
    for (size_t i = 0; i < SUPPORTED_SPEEDS_COUNT; ++i) {
        if (supported_speeds[i].bitmask == bitmask) {
            return supported_speeds[i].rpm;
        }
    }
    return 0;
}

unsigned create_bitmask_from_gearbox_state(const GearboxMicroSwitchState state) {
    unsigned bitmask = 0;
    bitmask |= (state.input.left_center << 11);
    bitmask |= (state.input.center << 10);
    bitmask |= (state.input.right << 9);
    bitmask |= (state.input.left << 8);
    bitmask |= (state.middle.left_center << 7);
    bitmask |= (state.middle.center << 6);
    bitmask |= (state.middle.right << 5);
    bitmask |= (state.middle.left << 4);
    bitmask |= (state.reducer.left_center << 3);
    bitmask |= (state.reducer.center << 2);
    bitmask |= (state.reducer.right << 1);
    bitmask |= state.reducer.left;
    return bitmask;
}

bool gearshift_needs_reverse(
    const CurrentAxisMicroSwitchState current_state, const TargetAxisMicroSwitchState target_state
) {
    if (target_state.right)
        return true;
    if (target_state.left)
        return false;
    return !(target_state.center && (current_state.left_center || current_state.right));
}

bool should_slow_down(
    CurrentAxisMicroSwitchState current_state, TargetAxisMicroSwitchState target_state
) {
    return target_state.center == true && current_state.center == true;
}
