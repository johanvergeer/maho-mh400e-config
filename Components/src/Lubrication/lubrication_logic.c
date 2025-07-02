#include "lubrication_logic.h"

#include <stdbool.h>

/**
 * @brief Determine the next state of the lubrication pump based on time and
 * input.
 *
 * @param time The current time in seconds.
 * @param input The input signals for the lubrication pump.
 * @param state The current lubrication state
 * @param config The lubrication pump config
 * @return The output signals for the lubrication pump.
 */
void lubricate(
    const float time,
    const LubricationSignals input,
    LubricationState *state,
    const LubricationConfig config
) {
    if (state->state == LUBRICATION_STATE_ERROR) {
        // Once the ERROR state is reached a hard reset is required.
        return;
    }

    if (config.is_enabled == false || input.is_motion_enabled == false) {
        state->state = LUBRICATION_STATE_DISABLED;
        return;
    }

    switch (state->state) {
        case LUBRICATION_STATE_INITIALIZING:
            state->building_pressure_start_time = time;
            state->lubrication_start_time = time;
            state->state = LUBRICATION_STATE_BUILDING_PRESSURE;
            break;
        // DISABLED and IDLE have the same logic, but they have a different
        // semantic meaning, which is used in the GUI.
        case LUBRICATION_STATE_DISABLED:
        case LUBRICATION_STATE_IDLE:
            if (time - state->last_cycle_end_time > config.interval) {
                state->building_pressure_start_time = time;
                state->lubrication_start_time = time;
                state->state = LUBRICATION_STATE_BUILDING_PRESSURE;
                break;
            }
            state->state = LUBRICATION_STATE_IDLE;
            break;
        case LUBRICATION_STATE_BUILDING_PRESSURE:
            if (input.is_pressure_ok) {
                state->state = LUBRICATION_STATE_LUBRICATING;
                state->lubrication_start_time = time;
                break;
            }
            if (time - state->building_pressure_start_time > config.pressure_timeout) {
                state->state = LUBRICATION_STATE_ERROR;
            }
            break;
        case LUBRICATION_STATE_LUBRICATING:
            if (time - state->lubrication_start_time > config.pressure_hold_time) {
                state->state = LUBRICATION_STATE_IDLE;
                state->last_cycle_end_time = time;
            }
            break;
        default:
            break;
    }
}
