#ifndef GEARBOX_LOGIC_H
#define GEARBOX_LOGIC_H
#include <stdbool.h>
#include <stddef.h>

/**
 * Commands sent to the machine in order to change the spindle speed.
 */
typedef struct {
    bool select_mid_position;
    bool backgear;
    bool mid_range;
    bool input_stage;
    bool reverse_direction;
    bool start;
    bool twitch_cw;
    bool twitch_ccw;
} SpindleSpeedControlCommands;

typedef struct {
    bool left_center;
    bool center;
    bool right;
    bool left;
} CurrentAxisMicroSwitchState;

typedef struct {
    bool center;
    bool right;
    bool left;
} TargetAxisMicroSwitchState;

typedef struct {
    CurrentAxisMicroSwitchState input;
    CurrentAxisMicroSwitchState middle;
    CurrentAxisMicroSwitchState reducer;
} GearboxMicroSwitchState;

/**
 * @brief Define whether the gearshift needs to be reversed
 *
 * @details
 *  https://forum.linuxcnc.org/12-milling/33035-retrofitting-a-1986-maho-mh400e?start=460#117021
 *  1. if u need to go to the left then turn CW
 *  2. if u need to go to the right than turn CCW
 *  3. if u need to go to the middle and Left-Center is 1 then turn ccw else CW
 *
 * ┌───┐
 * ┘   └──────────────── left
 *         ┌───┐
 * ────────┘   └──────── center
 *                 ┌───┐
 * ────────────────┘   └ right
 *           ┌──────────
 * ──────────┘           left-center
 *
 *
 * @param current_state of a single gearbox axis
 * @param target_state of a single gearbox axis
 * @return true if the gearshift needs to be reversed, false otherwise.
 */
bool gearshift_needs_reverse(
    CurrentAxisMicroSwitchState current_state, TargetAxisMicroSwitchState target_state
);
typedef struct {
    unsigned rpm;
    unsigned bitmask;
} SupportedSpeed;

/**
 * Defines an array of supported speed configurations, where each configuration maps a specific
 * revolutions per minute (RPM) value to its corresponding bitmask representation.
 *
 * The bitmask represents the positions of the 15 gearbox micro switches
 *
 * Each entry in the array consists of:
 * - `rpm`: The RPM value associated with the speed configuration.
 * - `bitmask`: A 12-bit representation (or similar width) encoding the configuration for the given
 * RPM.
 *
 * These mappings are used to determine the appropriate settings for various operation modes
 * or conditions based on the RPM or bitmask values.
 */
extern SupportedSpeed supported_speeds[];

/**
 * Represents the total number of supported speed configurations available in the `supported_speeds`
 * array.
 */
const size_t SUPPORTED_SPEEDS_COUNT;

/**
 * Retrieves the bitmask value corresponding to a specified revolutions per minute (RPM).
 *
 * @param rpm The RPM used to search for the corresponding bitmask value.
 * @return The bitmask value corresponding to the given RPM, or 0 if no match is found.
 */
unsigned get_bitmask_from_rpm(unsigned rpm);

/**
 * Retrieves the revolutions per minute (RPM) value corresponding to a specified bitmask.
 *
 * @param bitmask The bitmask used to search for the corresponding RPM value.
 * @return The RPM value corresponding to the given bitmask, or 0 if no match is found.
 */
unsigned get_rpm_from_bitmask(unsigned bitmask);

/**
 *
 * @param state The current state of the gearbox micro switches
 * @return The bitmask of the current gearbox micro switches state
 */
unsigned create_bitmask_from_gearbox_state(GearboxMicroSwitchState state);

/**
 * Determine whether the gear motor should slow down to avoid overshooting the center gear position.
 *
 * Slowing down is only relevant when the target position is `center`. In that case,
 * the motor should decelerate when the `center` micro switch is active and stop
 * as soon as the `center_left` micro switch changes state.
 *
 * @param current_state The current micro switch state for this axis
 * @param target_state The desired micro switch state for this axis
 * @return true if the motor should slow down, false otherwise
 */
bool should_slow_down(
    CurrentAxisMicroSwitchState current_state, TargetAxisMicroSwitchState target_state
);

#endif // GEARBOX_LOGIC_H
