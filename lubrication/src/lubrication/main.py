import datetime
import time

import qtvcp.logger

from lubrication.adapters.linuxcnc import CommandAdapter, HalAdapter, IniAdapter, StatAdapter
from lubrication.controller import LubePumpController, LubricationTimer, AxisMotionTracker


def main() -> None:
    hal = HalAdapter()
    ini = IniAdapter()

    controller = LubePumpController(
        hal,
        StatAdapter(),
        CommandAdapter(),
        ini,
        qtvcp.logger.getLogger("LUBRICATION"),
        LubricationTimer(AxisMotionTracker(hal, ini), ini, datetime.datetime.now().timestamp()),
    )
    try:
        while True:
            controller.update()
            time.sleep(0.1)
    except KeyboardInterrupt:
        controller.hal.deactivate_pump()
        raise SystemExit


if __name__ == "__main__":
    main()
