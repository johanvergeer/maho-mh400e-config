#!/usr/bin/env python3

import hal
import time
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

        self.pump_interval = 16 * 60  # seconds
        self.last_pump_time = time.time()
        self.error_state = False

        self.pump_running = False
        self.pump_start_time = None
        self.pressure_detected_time = None
        self.waiting_for_pressure = False
        self.pressure_hold_phase = False

        self.stat = linuxcnc.stat()
        self.command = linuxcnc.command()

    def send_qtdragon_error(self, message: str) -> None:
        """Send error message to QtDragon HD"""
        self.command.error_msg(message)

    def update(self) -> None:
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
            elif (now - self.pump_start_time) > 60:
                _logger.error("Lubrication pressure not reached within 60 seconds")
                self.send_qtdragon_error("Lubrication pressure not reached within 60 seconds!")
                self.halcomp["error_active"] = True
                self.halcomp["pump_active"] = False
                self.pump_running = False
                self.waiting_for_pressure = False
                self.error_state = True
                return

        # Hold pressure for 15 seconds
        if self.pressure_hold_phase and (now - self.pressure_detected_time) >= 15:
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
