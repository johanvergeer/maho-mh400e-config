#include <stdbool.h>

/* The possible states of the lubrication pump */
typedef enum {
    INITIALIZING, /* The initial state before the first lubrication cycle starts. */
    IDLE, /* The lubrication pump is idle. */
    BUILDING_PRESSURE, /* The lubrication pump is building pressure. */
    LUBRICATING, /* The lubrication pump is actively lubricating. */
    ERROR /* The lubrication pump is in an error state. */
} LubricationState;

/* The configuration parameters for the lubrication pump */
typedef struct {
    bool isEnabled; /* Whether the lubrication pump is enabled. */
    int interval; /* The interval between lubrication cycles (in seconds). */
    int pressureTimeout; /* The maximum time allowed to build pressure (in seconds). */
    int pressureHoldTime; /* The time to keep the pump running after pressure is reached (in seconds). */
} LubricationConfig;

/* The input signals for the lubrication pump logic */
typedef struct {
    bool isMotionEnabled; /* Whether the CNC controller is enabled (false when the e-stop is pressed). */
    bool isPressureOk; /* Whether the required pressure has been reached. */
} LubricationPumpInput;

/* The output signals from the lubrication pump logic */
typedef struct {
    bool isEnabled; /* Whether the lubrication pump should be enabled. */
    LubricationState state; /* The current lubrication state. */
} LubricationPumpOutput;

/**
 * @brief Determine the next state of the lubrication pump based on time and input.
 *
 * @param time The current time in seconds.
 * @param input The input signals for the lubrication pump.
 * @return The output signals for the lubrication pump.
 */
LubricationPumpOutput lubricate(float time, LubricationPumpInput input) {
}
