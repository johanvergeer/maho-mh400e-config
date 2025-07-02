#ifndef GEARS_H
#define GEARS_H
#include <stdbool.h>
#include <stddef.h>

typedef struct {
    unsigned rpm;
    unsigned bitmask;
} SupportedSpeed;

typedef struct {
    bool input_left_center;
    bool input_center;
    bool input_right;
    bool input_left;
    bool middle_left_center;
    bool middle_center;
    bool middle_right;
    bool middle_left;
    bool reducer_left_center;
    bool reducer_center;
    bool reducer_right;
    bool reducer_left;
} GearboxMicroSwitchState;

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

#endif // GEARS_H
