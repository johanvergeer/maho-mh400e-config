import datetime

import pytest

from lubrication.adapters.interfaces import HalInterface, IniInterface
from lubrication.controller import AxisMotionTracker


class DummyHal(HalInterface):
    def __init__(self, x: float = 0, y: float = 0, z: float = 0):
        self._x_axis_position = x
        self._y_axis_position = y
        self._z_axis_position = z

    @property
    def x_axis_position(self) -> float:
        return self._x_axis_position

    @property
    def y_axis_position(self) -> float:
        return self._y_axis_position

    @property
    def z_axis_position(self) -> float:
        return self._z_axis_position


class DummyIni(IniInterface):
    movement_threshold = 0.05
    movement_window_seconds = 1.0
    update_interval = 0.1


def test_not_enough_movements_registered_yet() -> None:
    hal = DummyHal(x=1.0, y=2.0, z=3.0)
    tracker = AxisMotionTracker(hal, DummyIni())

    # only register a single movement.
    tracker.update(datetime.datetime.now())

    assert not tracker.has_moved_recently


def test_no_movement_detected() -> None:
    hal = DummyHal(x=1.0, y=2.0, z=3.0)
    tracker = AxisMotionTracker(hal, DummyIni())

    now = datetime.datetime.now()
    for i in range(10):
        tracker.update(now + datetime.timedelta(seconds=i * DummyIni.update_interval))

    assert not tracker.has_moved_recently


def test_movement_detected() -> None:
    hal = DummyHal(x=0.0, y=0.0, z=0.0)
    tracker = AxisMotionTracker(hal, DummyIni())

    now = datetime.datetime.now()
    tracker.update(now)

    hal._x_axis_position = 0.1  # movement above threshold
    tracker.update(now + datetime.timedelta(seconds=DummyIni.movement_window_seconds))

    assert tracker.has_moved_recently


def test_slow_creep_below_threshold() -> None:
    hal = DummyHal(x=0.0)
    tracker = AxisMotionTracker(hal, DummyIni())

    now = datetime.datetime.now()

    # Simulate 10 small steps of 0.004 (total displacement = 0.04, just below threshold)
    for i in range(10):
        hal._x_axis_position = i * 0.004
        tracker.update(now + datetime.timedelta(seconds=i * DummyIni.update_interval))

    assert not tracker.has_moved_recently


def test_slow_creep_above_threshold() -> None:
    hal = DummyHal(x=0.0)
    tracker = AxisMotionTracker(hal, DummyIni())

    now = datetime.datetime.now()

    # Simulate 10 small steps of 0.006 (total displacement = 0.06, just above threshold)
    for i in range(10):
        hal._x_axis_position = i * 0.006
        tracker.update(now + datetime.timedelta(seconds=i * DummyIni.update_interval))

    assert tracker.has_moved_recently


class InvalidIni(IniInterface):
    movement_threshold = 0.05
    movement_window_seconds = 0.5
    update_interval = 1.0  # Invalid: greater than movement_window_seconds


def test_invalid_update_interval_raises_value_error() -> None:
    hal = DummyHal()

    with pytest.raises(ValueError, match="update_interval cannot exceed movement_window_seconds"):
        AxisMotionTracker(hal, InvalidIni())
