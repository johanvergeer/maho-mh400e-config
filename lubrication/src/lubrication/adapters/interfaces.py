from abc import ABC
from typing import Protocol


class HalInterface(ABC):
    @property
    def is_machine_on(self) -> bool:
        raise NotImplementedError

    @property
    def is_pressure_ok(self) -> bool:
        raise NotImplementedError

    @property
    def x_axis_position(self) -> float:
        raise NotImplementedError

    @property
    def y_axis_position(self) -> float:
        raise NotImplementedError

    @property
    def z_axis_position(self) -> float:
        raise NotImplementedError

    def activate_pump(self) -> None:
        raise NotImplementedError

    def deactivate_pump(self) -> None:
        raise NotImplementedError

    def activate_error(self) -> None:
        raise NotImplementedError

    def deactivate_error(self) -> None:
        raise NotImplementedError

    def __str__(self):
        return (
            f"HAL status:\n"
            f"  Machine ON     : {self.is_machine_on}\n"
            f"  Pressure OK    : {self.is_pressure_ok}\n"
            f"  X-axis position: {self.x_axis_position:.3f}\n"
            f"  Y-axis position: {self.y_axis_position:.3f}\n"
            f"  Z-axis position: {self.z_axis_position:.3f}"
        )

    def __repr__(self):
        return (
            f"{self.__class__.__name__}("
            f"is_machine_on={self.is_machine_on}, "
            f"is_pressure_ok={self.is_pressure_ok}, "
            f"x={self.x_axis_position:.3f}, "
            f"y={self.y_axis_position:.3f}, "
            f"z={self.z_axis_position:.3f})"
        )

    def __hash__(self):
        return hash((
            self.is_machine_on,
            self.is_pressure_ok,
            round(self.x_axis_position, 2),
            round(self.y_axis_position, 2),
            round(self.z_axis_position, 2),
        ))


class StatInterface(ABC):
    def poll(self) -> None:
        raise NotImplementedError


class CommandInterface(ABC):
    def text_msg(self, message: str) -> None:
        raise NotImplementedError

    def error_msg(self, message: str) -> None:
        raise NotImplementedError


class IniInterface(ABC):
    @property
    def debug_mode(self) -> bool:
        """Indicates whether the application is running in debug mode.
    
        When True, additional debug information and logging might be enabled
        to assist in troubleshooting or development. When False, the application
        operates in normal mode without additional debugging output.
        """
        raise NotImplementedError

    @property
    def update_interval(self) -> float:
        """The interval at which to run the lubrication controller update loop."""
        raise NotImplementedError

    @property
    def is_lubrication_enabled(self) -> bool:
        """Indicates whether the lubrication system is enabled.

        When True, the lubrication control logic is active, and the system
        will perform lubrication cycles. When False, the lubrication logic
        is disabled, and no lubrication actions are performed.
        """
        raise NotImplementedError

    @property
    def interval_consecutive_movement(self) -> int:
        """The time interval after which lubrication should run if any movements
        occurred within this timeframe.

        This ensures that lubrication is triggered periodically based on
        movements within a defined time window.
        """
        raise NotImplementedError

    @property
    def pressure_timeout(self) -> int:
        """The maximum time in seconds allowed for the lubrication system to reach the required
         pressure.

        If the required pressure is not achieved within this time frame, an error is triggered
        to indicate a potential malfunction or issue with the lubrication system.
        """
        raise NotImplementedError

    @property
    def pressure_hold_time(self) -> int:
        """The duration, in seconds, that the lubrication pump continues to operate
        after the required pressure has been reached.

        This ensures thorough lubrication by maintaining pump operation
        for a defined period after pressure is established.
        """
        raise NotImplementedError

    @property
    def movement_threshold(self) -> float:
        """Minimum distance an axis must move to be considered actual movement.

        This value filters out noise or mechanical backlash by ignoring movements
        smaller than this threshold.
        """
        raise NotImplementedError

    @property
    def movement_window_seconds(self) -> float:
        """Time window in which the minimum movement must occur to count as movement.

        If the axes move too slowly, such as during creeping, the total movement
        must still exceed the threshold within this time window to be recognized.
        """
        raise NotImplementedError

    def __str__(self):
        return (
            "Ini settings:\n"
            f"  Lubrication enabled        : {self.is_lubrication_enabled}\n"
            f"  Update interval            : {self.update_interval:.3f} s\n"
            f"  Interval for movement lube : {self.interval_consecutive_movement} s\n"
            f"  Pressure timeout           : {self.pressure_timeout} s\n"
            f"  Pressure hold time         : {self.pressure_hold_time} s\n"
            f"  Movement threshold         : {self.movement_threshold:.4f} mm\n"
            f"  Movement window            : {self.movement_window_seconds:.3f} s"
        )

    def __repr__(self):
        return (
            f"{self.__class__.__name__}("
            f"is_lubrication_enabled={self.is_lubrication_enabled}, "
            f"update_interval={self.update_interval:.3f}, "
            f"interval_consecutive_movement={self.interval_consecutive_movement}, "
            f"pressure_timeout={self.pressure_timeout}, "
            f"pressure_hold_time={self.pressure_hold_time}, "
            f"movement_threshold={self.movement_threshold:.4f}, "
            f"movement_window_seconds={self.movement_window_seconds:.3f})"
        )


class Logger(Protocol):
    def info(self, msg: str) -> None: ...
    def warning(self, msg: str) -> None: ...
    def error(self, msg: str) -> None: ...
