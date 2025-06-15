//
// Created by Johan Vergeer on 15/06/2025.
//
#include <stdbool.h>

#ifndef LUBRICATION_LOGIC_H
#define LUBRICATION_LOGIC_H

/* The possible states of the lubrication pump */
typedef enum {
    INITIALIZING = 0, /* The initial state before the first lubrication cycle starts. */
    DISABLED = 1, /* The lubrication pump is disabled. */
    IDLE = 2, /* The lubrication pump is idle. */
    BUILDING_PRESSURE = 3, /* The lubrication pump is building pressure. */
    LUBRICATING = 4, /* The lubrication pump is actively lubricating. */
    ERROR = 5 /* The lubrication pump is in an error state. */
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

LubricationPumpOutput lubricate(
    float time,
    LubricationPumpInput input,
    LubricationConfig config
);

#endif //LUBRICATION_LOGIC_H
