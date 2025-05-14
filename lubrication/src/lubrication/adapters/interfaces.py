from typing import Protocol


class HalInterface(Protocol):
    @property
    def is_machine_on(self) -> bool: ...

    @property
    def is_pressure_ok(self) -> bool: ...

    @property
    def x_axis_position(self) -> float: ...

    @property
    def y_axis_position(self) -> float: ...

    @property
    def z_axis_position(self) -> float: ...

    def activate_pump(self) -> None: ...

    def deactivate_pump(self) -> None: ...

    def activate_error(self) -> None: ...

    def deactivate_error(self) -> None: ...


class StatInterface(Protocol):
    def poll(self) -> None: ...


class CommandInterface(Protocol):
    def text_msg(self, message: str) -> None: ...

    def error_msg(self, message: str) -> None: ...


class IniInterface(Protocol):
    @property
    def update_interval(self) -> float:
        """The interval at which to run the lubrication controller update loop."""

    @property
    def is_lubrication_enabled(self) -> bool: ...

    @property
    def pump_interval(self) -> int: ...

    @property
    def pressure_timeout(self) -> int: ...

    @property
    def pressure_hold_time(self) -> int: ...

    @property
    def movement_threshold(self) -> float:
        """Minimum distance an axis must move to be considered actual movement.

        This value filters out noise or mechanical backlash by ignoring movements
        smaller than this threshold.
        """

    @property
    def movement_window_seconds(self) -> float:
        """Time window in which the minimum movement must occur to count as movement.

        If the axes move too slowly, such as during creeping, the total movement
        must still exceed the threshold within this time window to be recognized.
        """


class Logger(Protocol):
    def info(self, msg: str) -> None: ...
    def warning(self, msg: str) -> None: ...
    def error(self, msg: str) -> None: ...
