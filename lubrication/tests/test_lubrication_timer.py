import datetime
from unittest.mock import create_autospec

from freezegun import freeze_time

from lubrication.adapters.interfaces import IniInterface
from lubrication.controller import AxisMotionTracker, LubricationTimer


def test_should_lubricate_when_machine_starts():
    # Mock the AxisMotionTracker
    axis_motion_tracker = create_autospec(spec=AxisMotionTracker)
    axis_motion_tracker.has_moved_recently = False
    ini = create_autospec(spec=IniInterface)

    # Initialize the LubricationTimer with machine_start_lubricate=True
    timer = LubricationTimer(
        axis_motion_tracker=axis_motion_tracker,
        ini=ini,
        now=datetime.datetime.now().timestamp(),
        machine_start_lubricate=True,
    )

    assert timer.should_lubricate(datetime.datetime.now().timestamp()) is True
    assert timer.machine_start_lubricate is False


def test_should_lubricate_after_consecutive_movement_interval_has_expired_and_movements_are_detected():  # noqa: E501
    # Mock the AxisMotionTracker
    axis_motion_tracker = create_autospec(spec=AxisMotionTracker)
    axis_motion_tracker.has_moved_recently = False
    ini = create_autospec(spec=IniInterface)
    ini.interval_consecutive_movement = 60 * 16  # 16 minutes

    with freeze_time("2023-01-01 00:00:00"):
        # Initialize the LubricationTimer with machine_start_lubricate=True
        timer = LubricationTimer(
            axis_motion_tracker=axis_motion_tracker,
            ini=ini,
            now=datetime.datetime.now().timestamp(),
            machine_start_lubricate=False,
        )

    axis_motion_tracker.has_moved_recently = True
    with freeze_time("2023-01-01 00:16:01"):
        assert timer.should_lubricate(datetime.datetime.now().timestamp()) is True


def test_should_lubricate_after_consecutive_movement_interval_has_expired_and_movements_are_detected_previously():  # noqa: E501
    # Mock the AxisMotionTracker
    axis_motion_tracker = create_autospec(spec=AxisMotionTracker)
    axis_motion_tracker.has_moved_recently = False
    ini = create_autospec(spec=IniInterface)
    ini.interval_consecutive_movement = 60 * 16  # 16 minutes

    with freeze_time("2023-01-01 00:00:00"):
        # Initialize the LubricationTimer with machine_start_lubricate=True
        timer = LubricationTimer(
            axis_motion_tracker=axis_motion_tracker,
            ini=ini,
            now=datetime.datetime.now().timestamp(),
            machine_start_lubricate=False,
        )

    axis_motion_tracker.has_moved_recently = True
    with freeze_time("2023-01-01 00:10:00"):
        assert timer.should_lubricate(datetime.datetime.now().timestamp()) is False

    axis_motion_tracker.has_moved_recently = False
    with freeze_time("2023-01-01 00:16:01"):
        assert timer.should_lubricate(datetime.datetime.now().timestamp()) is True


def test_should_not_lubricate_after_consecutive_movement_interval_has_expired_and_no_movements_are_detected():  # noqa: E501
    # Mock the AxisMotionTracker
    axis_motion_tracker = create_autospec(spec=AxisMotionTracker)
    axis_motion_tracker.has_moved_recently = False
    ini = create_autospec(spec=IniInterface)
    ini.interval_consecutive_movement = 60 * 16  # 16 minutes

    with freeze_time("2023-01-01 00:00:00"):
        # Initialize the LubricationTimer with machine_start_lubricate=True
        timer = LubricationTimer(
            axis_motion_tracker=axis_motion_tracker,
            ini=ini,
            now=datetime.datetime.now().timestamp(),
            machine_start_lubricate=False,
        )

    axis_motion_tracker.has_moved_recently = False
    with freeze_time("2023-01-01 00:16:01"):
        assert timer.should_lubricate(datetime.datetime.now().timestamp()) is False


def test_should_not_lubricate_when_machine_start_disabled_and_no_movement():
    axis_motion_tracker = create_autospec(spec=AxisMotionTracker)
    axis_motion_tracker.has_moved_recently = False
    ini = create_autospec(spec=IniInterface)
    ini.interval_consecutive_movement = 60 * 16

    with freeze_time("2023-01-01 00:00:00"):
        timer = LubricationTimer(
            axis_motion_tracker=axis_motion_tracker,
            ini=ini,
            now=datetime.datetime.now().timestamp(),
            machine_start_lubricate=False,
        )
        assert timer.should_lubricate(datetime.datetime.now().timestamp()) is False

    with freeze_time("2023-01-01 00:10:00"):
        assert timer.should_lubricate(datetime.datetime.now().timestamp()) is False


def test_should_lubricate_at_exact_threshold_with_movement():
    axis_motion_tracker = create_autospec(spec=AxisMotionTracker)
    axis_motion_tracker.has_moved_recently = True
    ini = create_autospec(spec=IniInterface)
    ini.interval_consecutive_movement = 60 * 16

    with freeze_time("2023-01-01 00:00:00"):
        now = datetime.datetime.now().timestamp()
        timer = LubricationTimer(
            axis_motion_tracker=axis_motion_tracker, ini=ini, now=now, machine_start_lubricate=False
        )

    with freeze_time("2023-01-01 00:16:00"):
        assert timer.should_lubricate(datetime.datetime.now().timestamp()) is True


def test_should_lubricate_only_once_without_reset():
    axis_motion_tracker = create_autospec(spec=AxisMotionTracker)
    axis_motion_tracker.has_moved_recently = True
    ini = create_autospec(spec=IniInterface)
    ini.interval_consecutive_movement = 60 * 16

    start = datetime.datetime(2023, 1, 1, 0, 0, 0)
    with freeze_time(start):
        timer = LubricationTimer(
            axis_motion_tracker, ini, now=start.timestamp(), machine_start_lubricate=False
        )

    with freeze_time(start + datetime.timedelta(minutes=16, seconds=1)):
        assert timer.should_lubricate(datetime.datetime.now().timestamp()) is True
        assert timer.should_lubricate(datetime.datetime.now().timestamp()) is False


def test_reset_midway_resets_lubrication_timer():
    axis_motion_tracker = create_autospec(spec=AxisMotionTracker)
    axis_motion_tracker.has_moved_recently = True
    ini = create_autospec(spec=IniInterface)
    ini.interval_consecutive_movement = 60 * 16

    t0 = datetime.datetime(2023, 1, 1, 0, 0, 0)
    t_reset = t0 + datetime.timedelta(minutes=10)
    t_check_no_lubricate = t0 + datetime.timedelta(minutes=20)
    t_check_lubricate = t0 + datetime.timedelta(minutes=26)

    with freeze_time(t0):
        timer = LubricationTimer(
            axis_motion_tracker, ini, now=t0.timestamp(), machine_start_lubricate=False
        )

    with freeze_time(t_reset):
        timer.reset(t_reset.timestamp())

    with freeze_time(t_check_no_lubricate):
        assert timer.should_lubricate(t_check_no_lubricate.timestamp()) is False

    with freeze_time(t_check_lubricate):
        assert timer.should_lubricate(t_check_lubricate.timestamp()) is True
