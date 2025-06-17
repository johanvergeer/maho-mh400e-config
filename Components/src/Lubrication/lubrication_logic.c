#include <stdbool.h>
#include "lubrication_logic.h"


/**
 * @brief Determine the next state of the lubrication pump based on time and input.
 *
 * @param time The current time in seconds.
 * @param input The input signals for the lubrication pump.
 * @param state
 * @param config The lubrication pump config
 * @return The output signals for the lubrication pump.
 */
void lubricate(
    const float time,
    const LubricationSignals input,
    LubricationState *state,
    const LubricationConfig config
) {
    if (state->state == ERROR) {
        // Once the ERROR state is reached a hard reset is required.
        return;
    }

    if (config.isEnabled == false || input.isMotionEnabled == false) {
        state->state = DISABLED;
        return;
    }

    switch (state->state) {
        case INITIALIZING:
            state->buildingPressureStartTime = time;
            state->lubricationStartTime = time;
            state->state = BUILDING_PRESSURE;
            break;
        case DISABLED:
        case IDLE:
            if (time - state->buildingPressureStartTime > config.interval) {
                state->buildingPressureStartTime = time;
                state->lubricationStartTime = time;
                state->state = BUILDING_PRESSURE;
                break;
            }
            state->state = IDLE;
            break;
        case BUILDING_PRESSURE:
            if (input.isPressureOk) {
                state->state = LUBRICATING;
                state->lubricationStartTime = time;
                break;
            }
            if (time - state->buildingPressureStartTime > config.pressureTimeout) {
                state->state = ERROR;
            }
            break;
        case LUBRICATING:
            if (time - state->lubricationStartTime > config.pressureHoldTime) {
                state->state = IDLE;
            }
            break;
        default:
            break;
    }
}
