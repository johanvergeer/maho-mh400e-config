#include <stdbool.h>
#include "lubrication_logic.h"

static float lastStateChangeTime = 0.0f;
static LubricationPumpOutput output = {INITIALIZING};

/**
 * @brief Determine the next state of the lubrication pump based on time and input.
 *
 * @param time The current time in seconds.
 * @param input The input signals for the lubrication pump.
 * @param config The lubrication pump config
 * @return The output signals for the lubrication pump.
 */
LubricationPumpOutput lubricate(
    const float time,
    const LubricationPumpInput input,
    const LubricationConfig config
) {
    if (output.state == ERROR) {
        // Once the ERROR state is reached a hard reset is required.
        return output;
    }

    if (config.isEnabled == false || input.isMotionEnabled == false) {
        output.state = DISABLED;
        return output;
    }

    switch (output.state) {
        case INITIALIZING:
            lastStateChangeTime = time;
            output.state = BUILDING_PRESSURE;
            break;
        case BUILDING_PRESSURE:
            if (input.isPressureOk) {
                output.state = LUBRICATING;
                break;
            }
            if ((time - lastStateChangeTime) > config.pressureTimeout) {
                output.state = ERROR;
                break;
            }
        default:
            break;
    }

    return output;
}

void lubrication_reset(void) {
    lastStateChangeTime = 0.0f;
    output.state = INITIALIZING;
}
