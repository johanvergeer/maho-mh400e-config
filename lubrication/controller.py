#!/usr/bin/env python3
import os
import time
from distutils.util import strtobool
from typing import Protocol

import hal
import linuxcnc
import qtvcp.logger


class HalAdapter:
    def __init__(self) -> None:
        self.halcomp = hal.component("lube_pump")
        self.halcomp.newpin("machine_on", hal.HAL_BIT, hal.HAL_IN)
        self.halcomp.newpin("pressure_ok", hal.HAL_BIT, hal.HAL_IN)
        self.halcomp.newpin("pump_active", hal.HAL_BIT, hal.HAL_OUT)
        self.halcomp.newpin("error_active", hal.HAL_BIT, hal.HAL_OUT)

        self.halcomp.ready()

    @property
    def is_machine_on(self) -> bool:
        return self.halcomp["machine_on"]

    @property
    def is_pump_active(self) -> bool:
        return self.halcomp["pump_active"]

    @is_pump_active.setter
    def is_pump_active(self, value: bool) -> None:
        self.halcomp["pump_active"] = value

    @property
    def is_error_active(self) -> bool:
        return self.halcomp["error_active"]

    @is_error_active.setter
    def is_error_active(self, value: bool) -> None:
        self.halcomp["error_active"] = value

    @property
    def is_pressure_ok(self) -> bool:
        return self.halcomp["pressure_ok"]

    @is_pressure_ok.setter
    def is_pressure_ok(self, value: bool) -> None:
        self.halcomp["pressure_ok"] = value


class StatAdapter:
    def __init__(self) -> None:
        self.stat = linuxcnc.stat()

    def poll(self) -> None:
        self.stat.poll()


class CommandAdapter:
    def __init__(self) -> None:
        self.command = linuxcnc.command()

    def text_msg(self, message: str) -> None:
        self.command.text_msg(message)

    def error_msg(self, message: str) -> None:
        self.command.error_msg(message)


class IniAdapter:
    def __init__(self) -> None:
        self.inifile = linuxcnc.ini(self._get_inifile_path())
        self._inifile_section = "LUBRICATION"

    def _get_inifile_path(self) -> str:
        inifile_path = os.environ.get("INI_FILE_NAME")
        if not inifile_path:
            raise RuntimeError("INI_FILE_NAME environment variable not set")
        return inifile_path

    @property
    def is_lubrication_enabled(self) -> bool:
        return bool(strtobool(self.inifile.find(self._inifile_section, "ENABLED") or "false"))

    @property
    def pump_interval(self) -> int:
        return int(self.inifile.find(self._inifile_section, "PUMP_INTERVAL_1"))

    @property
    def pressure_timeout(self) -> int:
        return int(self.inifile.find(self._inifile_section, "PRESSURE_TIMEOUT"))

    @property
    def pressure_hold_time(self) -> int:
        return int(self.inifile.find(self._inifile_section, "PRESSURE_HOLD_TIME"))


class Logger(Protocol):
    def info(self, msg: str) -> None: ...
    def warning(self, msg: str) -> None: ...
    def error(self, msg: str) -> None: ...


class LubricationTimer:
    def __init__(self, interval_seconds: float, now: float = None):
        self.interval = interval_seconds
        self._last_reset = now if now is not None else time.time()

    def reset(self, now: float = None) -> None:
        self._last_reset = now if now is not None else time.time()

    def should_lubricate(self, now: float = None) -> bool:
        now = now if now is not None else time.time()
        return (now - self._last_reset) >= self.interval

    def elapsed(self, now: float = None) -> float:
        now = now if now is not None else time.time()
        return now - self._last_reset


class LubricationState:
    def __init__(self):
        self.pump_running = False
        self.pump_start_time = None
        self.pressure_detected_time = None
        self.waiting_for_pressure = False
        self.pressure_hold_phase = False
        self.error_active = False

    def reset(self) -> None:
        self.pump_running = False
        self.pump_start_time = None
        self.pressure_detected_time = None
        self.waiting_for_pressure = False
        self.pressure_hold_phase = False
        self.error_active = False

    def should_abort_due_to_timeout(self, now: float, timeout: float) -> bool:
        return (
            self.pump_running
            and self.waiting_for_pressure
            and (now - self.pump_start_time) > timeout
        )

    def cycle_complete(self, now: float, hold_time: float) -> bool:
        return self.pressure_hold_phase and (now - self.pressure_detected_time) >= hold_time

    def enter_error_state(self) -> None:
        self.pump_running = False
        self.waiting_for_pressure = False
        self.error_active = True

    def start_cycle(self, now: float) -> None:
        self.pump_start_time = now
        self.pump_running = True
        self.waiting_for_pressure = True
        self.pressure_detected_time = None
        self.pressure_hold_phase = False

    def pressure_reached(self, now: float) -> None:
        self.pressure_detected_time = now
        self.waiting_for_pressure = False
        self.pressure_hold_phase = True

    def complete_cycle(self) -> None:
        self.pump_running = False
        self.pressure_hold_phase = False


class LubePumpController:
    def __init__(
        self,
        hal_adapter: HalAdapter,
        stat_adapter: StatAdapter,
        command_adapter: CommandAdapter,
        ini_adapter: IniAdapter,
        logger: Logger,
        timer: LubricationTimer,
    ) -> None:
        self.logger = logger
        self.logger.info("Initializing lubrication")
        self.hal = hal_adapter
        self.stat = stat_adapter
        self.command = command_adapter
        self.ini = ini_adapter
        self.state = LubricationState()
        self.timer = timer

        if not self.ini.is_lubrication_enabled:
            self.logger.warning("Lubrication logic is disabled via INI file")
            self.command.text_msg("Lubrication logic is disabled via INI file")

    def update(self) -> None:
        if not self.ini.is_lubrication_enabled:
            return

        self.stat.poll()
        now = time.time()

        if not self.hal.is_machine_on:
            self.hal.is_pump_active = False
            self.hal.is_error_active = False
            self.state.reset()
            self.timer.reset(now)
            return

        if self.state.error_active:
            if self.hal.is_pressure_ok:
                self.logger.info("Pressure restored, clearing lubrication error")
                self.hal.is_error_active = False
                self.state.reset()
                self.timer.reset(now)
            else:
                return

        if self.timer.should_lubricate(now):
            self.logger.info("Starting lubrication pump")
            self.hal.is_pump_active = True
            self.state.start_cycle(now)
            self.timer.reset(now)

        if self.state.pump_running and self.state.waiting_for_pressure:
            if self.hal.is_pressure_ok:
                self.state.pressure_reached(now)
                self.logger.info("Lubrication pressure reached, starting hold phase")
            elif self.state.should_abort_due_to_timeout(now, self.ini.pressure_timeout):
                self.logger.error(
                    f"Lubrication pressure not reached within {self.ini.pressure_timeout} seconds"
                )
                self.command.error_msg(
                    f"Lubrication pressure not reached within {self.ini.pressure_timeout} seconds!"
                )
                self.hal.is_error_active = True
                self.hal.is_pump_active = False
                self.state.enter_error_state()
                return

        if self.state.cycle_complete(now, self.ini.pressure_hold_time):
            self.logger.info("Lubrication cycle complete")
            self.hal.is_pump_active = False
            self.state.complete_cycle()
            self.timer.reset(now)


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
        controller.hal.is_pump_active = False
        raise SystemExit


if __name__ == "__main__":
    main()
