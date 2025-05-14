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


class Logger(Protocol):
    def info(self, msg: str) -> None: ...
    def warning(self, msg: str) -> None: ...
    def error(self, msg: str) -> None: ...
