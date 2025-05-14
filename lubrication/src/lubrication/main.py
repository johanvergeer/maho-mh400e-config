import datetime
import time

import qtvcp.logger

from lubrication.adapters.linuxcnc import CommandAdapter, HalAdapter, IniAdapter, StatAdapter
from lubrication.controller import AxisMotionTracker, LubePumpController, LubricationTimer


def main() -> None:
    logger = qtvcp.logger.getLogger("LUBRICATION")
    hal = HalAdapter()
    ini = IniAdapter()

    controller = LubePumpController(
        hal,
        StatAdapter(),
        CommandAdapter(),
        ini,
        logger,
        LubricationTimer(AxisMotionTracker(hal, ini), ini, datetime.datetime.now().timestamp()),
    )

    last_hash = None
    last_log_time = 0
    log_interval = 5.0  # seconden

    try:
        if ini.debug_mode:
            logger.info(f"INI Configuration: \n{str(ini)}")

        while True:
            now = time.time()
            current_hal_hash = hash(hal)

            if ini.debug_mode and current_hal_hash != last_hash and (now - last_log_time >= log_interval):
                logger.debug("HAL Status: %s", hal)
                last_hash = current_hal_hash
                last_log_time = now

            controller.update()
            time.sleep(0.1)
    except KeyboardInterrupt:
        controller.hal.deactivate_pump()
        raise SystemExit


if __name__ == "__main__":
    main()
