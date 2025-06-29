#ifndef LUBRICATION_LOGIC_H
#define LUBRICATION_LOGIC_H

#include <stdbool.h>

/* The possible states of the lubrication pump */
typedef enum {
    LUBRICATION_STATE_INITIALIZING = 0, /* The initial state before the first lubrication cycle starts. */
    LUBRICATION_STATE_DISABLED = 1, /* The lubrication pump is disabled. */
    LUBRICATION_STATE_IDLE = 2, /* The lubrication pump is idle. */
    LUBRICATION_STATE_BUILDING_PRESSURE = 3, /* The lubrication pump is building pressure. */
    LUBRICATION_STATE_LUBRICATING = 4, /* The lubrication pump is actively lubricating. */
    LUBRICATION_STATE_ERROR = 5 /* The lubrication pump is in an error state. */
} LubricationStates;

/* The configuration parameters for the lubrication pump */
typedef struct {
    const bool isEnabled; /* Whether the lubrication pump is enabled. */
    const float interval; /* The interval between lubrication cycles (in seconds). */
    const float pressureTimeout; /* The maximum time allowed to build pressure (in seconds). */
    const float pressureHoldTime; /* The time to keep the pump running after pressure is reached (in seconds). */
} LubricationConfig;

/* The input signals for the lubrication pump logic */
typedef struct {
    const bool isMotionEnabled; /* Whether the CNC controller is enabled (false when the e-stop is pressed). */
    const bool isPressureOk; /* Whether the required pressure has been reached. */
} LubricationSignals;

/* The output signals from the lubrication pump logic */
typedef struct {
    LubricationStates state; /* The current lubrication state. */
    double buildingPressureStartTime;
    double lubricationStartTime;
    double lastCycleEndTime;
} LubricationState;

void lubricate(
    float time,
    LubricationSignals input,
    LubricationState *state,
    LubricationConfig config
);

#endif //LUBRICATION_LOGIC_H
