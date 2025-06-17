#include <stdbool.h>
#include "lubrication_logic.h"

static float lastStateChangeTime = 0.0f;
static LubricationPumpOutput output = {false, INITIALIZING};

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
    if (config.isEnabled == false) {
        output.isEnabled = false;
        output.state = DISABLED;
        return output;
    }

    switch (output.state) {
        case INITIALIZING:
            if (input.isMotionEnabled) {
                lastStateChangeTime = time;
                output.state = BUILDING_PRESSURE;
                output.isEnabled = true;
            } else {
                output.isEnabled = false;
            }
            break;
        case BUILDING_PRESSURE:
            if (input.isPressureOk) {
                output.state = LUBRICATING;
                output.isEnabled = true;
                break;
            }
            if ((time - lastStateChangeTime) > config.pressureTimeout) {
                output.state = ERROR;
                output.isEnabled = false;
                break;
            }
        default:
            output.isEnabled = false;
            break;
    }

    return output;
}

void lubrication_reset(void) {
    lastStateChangeTime = 0.0f;
    output.isEnabled = false;
    output.state = INITIALIZING;
}
