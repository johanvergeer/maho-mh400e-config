#!/usr/bin/env python3
import time

from lubrication.adapters.interfaces import (
    CommandInterface,
    HalInterface,
    IniInterface,
    Logger,
    StatInterface,
)


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
        hal_adapter: HalInterface,
        stat_adapter: StatInterface,
        command_adapter: CommandInterface,
        ini_adapter: IniInterface,
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
            self.hal.deactivate_pump()
            self.hal.deactivate_error()
            self.state.reset()
            self.timer.reset(now)
            return

        if self.state.error_active:
            if self.hal.is_pressure_ok:
                self.logger.info("Pressure restored, clearing lubrication error")
                self.hal.deactivate_error()
                self.state.reset()
                self.timer.reset(now)
            else:
                return

        if self.timer.should_lubricate(now):
            self.logger.info("Starting lubrication pump")
            self.hal.activate_pump()
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
                self.hal.activate_error()
                self.hal.deactivate_pump()
                self.state.enter_error_state()
                return

        if self.state.cycle_complete(now, self.ini.pressure_hold_time):
            self.logger.info("Lubrication cycle complete")
            self.hal.deactivate_pump()
            self.state.complete_cycle()
            self.timer.reset(now)
