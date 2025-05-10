import time

import qtvcp.logger

from lubrication.adapters.linuxcnc import CommandAdapter, HalAdapter, IniAdapter, StatAdapter
from lubrication.controller import LubePumpController, LubricationTimer


def main() -> None:
    ini = IniAdapter()
    controller = LubePumpController(
        HalAdapter(),
        StatAdapter(),
        CommandAdapter(),
        ini,
        qtvcp.logger.getLogger("LUBRICATION"),
        LubricationTimer(interval_seconds=ini.pump_interval),
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
