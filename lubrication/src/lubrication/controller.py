#!/usr/bin/env python3
import dataclasses
import datetime
import math
import time
from collections import deque

from lubrication.adapters.interfaces import (
    CommandInterface,
    HalInterface,
    IniInterface,
    Logger,
    StatInterface,
)


@dataclasses.dataclass
class _PositionsAtTime:
    """Represents the recorded axis positions at a specific point in time."""

    time: datetime.datetime
    x_position: float
    y_position: float
    z_position: float


class AxisMotionTracker:
    """Tracks axis positions over time to detect recent movement.

    This tracker maintains a sliding window of recorded axis positions and determines
    whether significant movement has occurred within a configurable time window.
    """

    def __init__(self, hal: HalInterface, ini: IniInterface) -> None:
        """Initializes the motion tracker with HAL access and configuration.

        Args:
            hal: The HAL interface used to read axis positions.
            ini: The configuration interface providing movement thresholds and timing.
        """
        self._hal = hal
        self._ini = ini

        if self._ini.update_interval > self._ini.movement_window_seconds:
            raise ValueError(
                "update_interval cannot exceed movement_window_seconds "
                "— insufficient data points for movement tracking."
            )

        self._positions: deque[_PositionsAtTime] = deque(
            maxlen=math.ceil(self._ini.movement_window_seconds / self._ini.update_interval)
        )

    def update(self, time: datetime.datetime) -> None:
        """Records the current axis positions at the given time.

        Args:
            time: The timestamp associated with the position sample.
        """
        self._positions.append(
            _PositionsAtTime(
                time,
                self._hal.x_axis_position,
                self._hal.y_axis_position,
                self._hal.z_axis_position,
            )
        )

    @property
    def has_moved_recently(self) -> bool:
        """Indicates whether the axes have moved significantly in the recent time window.

        Returns:
            True if movement exceeded the configured threshold during the sliding window;
            False otherwise.
        """
        if len(self._positions) < 2:
            return False

        return self._distance_moved() > self._ini.movement_threshold

    def _distance_moved(self) -> float:
        """Calculates the maximum axis displacement within the tracked time window.

        Returns:
            The largest absolute movement among X, Y, and Z axes.
        """
        first = self._positions[0]
        last = self._positions[-1]
        return max(
            abs(last.x_position - first.x_position),
            abs(last.y_position - first.y_position),
            abs(last.z_position - first.z_position),
        )


class LubricationTimer:
    def __init__(
        self,
        axis_motion_tracker: AxisMotionTracker,
        ini: IniInterface,
        now: float,
        machine_start_lubricate: bool = True,
    ):
        """
        Initialize the lubrication timer.

        Args:
            axis_motion_tracker: Instance to track axis movements.
            now: The current time in minutes, passed by the caller.
            machine_start_lubricate: Whether to trigger lubrication on machine start.
        """
        self._axis_tracker = axis_motion_tracker
        self._ini = ini
        self._last_reset: float = now  # Timestamp of the last lubrication pulse
        self._axes_have_moved: bool = (
            False  # Whether the axes have moved since the last lubrication pulse
        )
        self._consecutive_motion_start: float | None = (
            None  # Start timestamp of consecutive axis motion
        )
        self.machine_start_lubricate = machine_start_lubricate

        # Configurable lubrication thresholds
        self.interval_consecutive_movement = self._ini.interval_consecutive_movement

    def reset(self, now: float) -> None:
        """
        Reset the lubrication timer and update the last reset timestamp.

        Args:
            now: The current time in minutes, passed by the caller.
        """
        self._last_reset = now
        self._consecutive_motion_start = None
        self._axes_have_moved = False

    def should_lubricate(self, now: float) -> bool:
        """
        Checks if lubrication should occur, based on the following conditions:
        1. Machine start triggers lubrication (one-time when enabled).
        2. Continuous axis movement for more than 16 minutes.
        3. An axis starts moving after previously no movement.

        Args:
            now: The current time in minutes, passed by the caller.

        Returns:
            True if lubrication is needed, False otherwise.
        """
        # Rule 1: Machine start lubrication
        if self.machine_start_lubricate:
            self.machine_start_lubricate = False  # Clear the flag after the first lubrication pulse
            return True

        if self._axis_tracker.has_moved_recently:
            self._axes_have_moved = True

        if self._last_reset <= now - self.interval_consecutive_movement and self._axes_have_moved:
            self.reset(now)
            return True

        return False


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

        self._was_machine_on = False

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
            self._was_machine_on = False
            return

        if self.state.error_active:
            if self.hal.is_pressure_ok:
                self.logger.info("Pressure restored, clearing lubrication error")
                self.hal.deactivate_error()
                self.state.reset()
                self.timer.reset(now)
            else:
                return

        if self.timer.should_lubricate(now) or not self._was_machine_on:
            self.logger.info("Starting lubrication pump")
            self.hal.activate_pump()
            self.state.start_cycle(now)
            self.timer.reset(now)
            self._was_machine_on = True

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
