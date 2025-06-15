#include <stdbool.h>
#include "lubrication_logic.h"
/**
 * @brief Determine the next state of the lubrication pump based on time and input.
 *
 * @param time The current time in seconds.
 * @param input The input signals for the lubrication pump.
 * @param config The lubrication pump config
 * @return The output signals for the lubrication pump.
 */
LubricationPumpOutput lubricate(
    float time,
    const LubricationPumpInput input,
    const LubricationConfig config
) {
    static LubricationPumpOutput output = {false, INITIALIZING};

    switch (output.state) {
        case INITIALIZING:
            if (input.isMotionEnabled && config.isEnabled) {
                output.state = BUILDING_PRESSURE;
                output.isEnabled = true;
            } else {
                output.isEnabled = false;
            }
            break;

        default:
            output.isEnabled = false;
            break;
    }

    return output;
}
