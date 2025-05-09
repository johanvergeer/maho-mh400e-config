#!/usr/bin/env python3

import os
import time
from distutils.util import strtobool

import hal
import linuxcnc
import qtvcp.logger

_logger = qtvcp.logger.getLogger("LUBRICATION")


class LubePumpController:
    def __init__(self) -> None:
        _logger.info("Initializing lubrication")
        self.halcomp = hal.component("lube_pump")
        self.halcomp.newpin("machine_on", hal.HAL_BIT, hal.HAL_IN)
        self.halcomp.newpin("pressure_ok", hal.HAL_BIT, hal.HAL_IN)
        self.halcomp.newpin("pump_active", hal.HAL_BIT, hal.HAL_OUT)
        self.halcomp.newpin("error_active", hal.HAL_BIT, hal.HAL_OUT)

        self.halcomp.ready()

        self.stat = linuxcnc.stat()
        self.command = linuxcnc.command()
        self.inifile = linuxcnc.ini(self._get_inifile_path())

        inifile_section = "LUBRICATION"
        self.enabled = bool(strtobool(self.inifile.find("LUBRICATION", "ENABLED") or "false"))
        if not self.enabled:
            _logger.warning("Lubrication logic is disabled via INI file")
            self.command.text_msg("Lubrication logic is disabled via INI file")
        self.pump_interval = int(self.inifile.find(inifile_section, "PUMP_INTERVAL_1"))
        self.pressure_timeout = int(self.inifile.find(inifile_section, "PRESSURE_TIMEOUT"))
        self.pressure_hold_time = int(self.inifile.find(inifile_section, "PRESSURE_HOLD_TIME"))

        self.last_pump_time = time.time() - self.pump_interval
        self.error_state = False

        self.pump_running = False
        self.pump_start_time = None
        self.pressure_detected_time = None
        self.waiting_for_pressure = False
        self.pressure_hold_phase = False

    def _get_inifile_path(self) -> str:
        inifile_path = os.environ.get("INI_FILE_NAME")
        if not inifile_path:
            raise RuntimeError("INI_FILE_NAME environment variable not set")
        return inifile_path

    def send_qtdragon_error(self, message: str) -> None:
        """Send error message to QtDragon HD"""
        self.command.error_msg(message)

    def update(self) -> None:
        if not self.enabled:
            return

        self.stat.poll()
        now = time.time()

        if not self.halcomp["machine_on"]:
            self.halcomp["pump_active"] = False
            self.halcomp["error_active"] = False
            self.error_state = False
            self.pump_running = False
            self.waiting_for_pressure = False
            self.pressure_hold_phase = False
            return

        if self.error_state:
            if self.halcomp["pressure_ok"]:
                _logger.info("Pressure restored, clearing lubrication error")
                self.halcomp["error_active"] = False
                self.error_state = False
                self.last_pump_time = now
            else:
                return  # blijf foutstatus houden

        # Start lubrication cycle
        if not self.pump_running and (now - self.last_pump_time) >= self.pump_interval:
            _logger.info("Starting lubrication pump")
            self.halcomp["pump_active"] = True
            self.pump_start_time = now
            self.pump_running = True
            self.waiting_for_pressure = True
            self.pressure_detected_time = None
            self.pressure_hold_phase = False

        # While waiting for pressure to build
        if self.pump_running and self.waiting_for_pressure:
            if self.halcomp["pressure_ok"]:
                self.pressure_detected_time = now
                self.waiting_for_pressure = False
                self.pressure_hold_phase = True
                _logger.info("Lubrication pressure reached, starting hold phase")
            elif (now - self.pump_start_time) > self.pressure_timeout:
                _logger.error(
                    f"Lubrication pressure not reached within {self.pressure_timeout} seconds"
                )
                self.send_qtdragon_error(
                    f"Lubrication pressure not reached within {self.pressure_timeout} seconds!"
                )
                self.halcomp["error_active"] = True
                self.halcomp["pump_active"] = False
                self.pump_running = False
                self.waiting_for_pressure = False
                self.error_state = True
                return

        if (
            self.pressure_hold_phase
            and (now - self.pressure_detected_time) >= self.pressure_hold_time
        ):
            _logger.info("Lubrication cycle complete")
            self.halcomp["pump_active"] = False
            self.pump_running = False
            self.pressure_hold_phase = False
            self.last_pump_time = now


def main() -> None:
    controller = LubePumpController()
    try:
        while True:
            controller.update()
            time.sleep(0.1)
    except KeyboardInterrupt:
        controller.halcomp["pump_active"] = False
        raise SystemExit


if __name__ == "__main__":
    main()
