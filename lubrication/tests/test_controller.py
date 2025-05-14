import datetime
import time
from unittest.mock import MagicMock, call, create_autospec

import pytest
from freezegun import freeze_time

from lubrication.adapters.interfaces import (
    CommandInterface,
    HalInterface,
    IniInterface,
    Logger,
    StatInterface,
)
from lubrication.controller import AxisMotionTracker, LubePumpController, LubricationTimer


def make_ini(lubrication_enabled: bool = True) -> IniInterface:
    ini = MagicMock(spec=IniInterface)

    ini.is_lubrication_enabled = lubrication_enabled
    ini.pressure_timeout = 60
    ini.pressure_hold_time = 15

    return ini


def make_timer(ini: IniInterface | None = None) -> LubricationTimer:
    ini = ini or make_ini()
    axis_motion_tracker = create_autospec(AxisMotionTracker)
    timer = LubricationTimer(axis_motion_tracker, ini, datetime.datetime.now().timestamp())

    return timer


def make_controller(
    *,
    hal: HalInterface = None,
    stat: StatInterface = None,
    command: CommandInterface = None,
    ini: IniInterface = None,
    logger: Logger = None,
    timer: LubricationTimer = None,
    was_machine_on: bool = True,
) -> LubePumpController:
    hal = hal or create_autospec(HalInterface, spec_set=True)
    stat = stat or create_autospec(StatInterface, spec_set=True)
    command = command or create_autospec(CommandInterface, spec_set=True)
    logger = logger or create_autospec(Logger, spec_set=True)
    timer = timer or make_timer()
    ini = ini or make_ini()

    controller = LubePumpController(hal, stat, command, ini, logger, timer)
    controller._was_machine_on = was_machine_on

    return controller


def test_controller_skips_lubrication_when_disabled():
    # Arrange
    hal = create_autospec(HalInterface, spec_set=True)
    logger = create_autospec(Logger, spec_set=True)
    command = create_autospec(CommandInterface, spec_set=True)
    controller = make_controller(
        hal=hal, command=command, ini=make_ini(lubrication_enabled=False), logger=logger
    )

    # Act
    controller.update()

    # Assert
    hal.activate_pump.assert_not_called()
    hal.deactivate_pump.assert_not_called()
    logger.info.assert_called_with("Initializing lubrication")
    logger.warning.assert_called_with("Lubrication logic is disabled via INI file")
    command.text_msg.assert_called_once()


@pytest.mark.parametrize("should_lubricate", [True, False])
def test_pump_starts_when_machine_turns_on_even_if_timer_does_not_trigger(should_lubricate) -> None:
    # Arrange
    hal = create_autospec(HalInterface, spec_set=True)
    stat = create_autospec(StatInterface, spec_set=True)
    command = create_autospec(CommandInterface, spec_set=True)
    logger = create_autospec(Logger, spec_set=True)

    hal.is_machine_on = True
    hal.is_pressure_ok = False

    ini = make_ini()

    # Timer geeft expliciet False terug, maar dat moet geen invloed hebben
    timer = create_autospec(LubricationTimer)
    timer.should_lubricate.return_value = should_lubricate

    controller = make_controller(
        hal=hal,
        stat=stat,
        command=command,
        ini=ini,
        logger=logger,
        timer=timer,
        was_machine_on=False,
    )

    # Act
    controller.update()

    # Assert
    hal.activate_pump.assert_called_once()
    logger.info.assert_any_call("Starting lubrication pump")
    timer.reset.assert_called_once()


def test_pump_stops_when_holdtime_has_elapsed():
    # Arrange
    hal = create_autospec(HalInterface, spec_set=True)
    logger = create_autospec(Logger, spec_set=True)

    hal.is_machine_on = True
    hal.is_pressure_ok = True

    timer = create_autospec(LubricationTimer, spec_set=True)
    timer.should_lubricate.return_value = False

    controller = make_controller(hal=hal, logger=logger, timer=timer)
    controller._was_machine_on = True

    # Eerst alles instellen op t=12:00:00
    with freeze_time("2024-01-01 12:00:00"):
        controller.state.start_cycle(time.time())
        controller.state.pressure_reached(time.time())

    # Daarna springen naar t=12:00:06 en update uitvoeren
    with freeze_time("2024-01-01 12:00:16"):
        controller.update()

    # Assert
    hal.deactivate_pump.assert_called_once()
    logger.info.assert_any_call("Lubrication cycle complete")
    timer.reset.assert_called_once()


def test_pump_continues_running_if_holdtime_not_elapsed_yet():
    # Arrange
    hal = create_autospec(HalInterface, spec_set=True)
    logger = create_autospec(Logger, spec_set=True)

    hal.is_machine_on = True
    hal.is_pressure_ok = True

    timer = create_autospec(LubricationTimer, spec_set=True)
    timer.should_lubricate.return_value = False

    controller = make_controller(hal=hal, logger=logger, timer=timer)
    with freeze_time("2024-01-01 12:00:00"):
        controller.state.start_cycle(time.time())  # cyclus is al gestart
        controller.state.pressure_reached(time.time())  # pas 5s sinds drukopbouw
        controller._was_machine_on = True

    # Act
    with freeze_time("2024-01-01 12:00:14"):
        controller.update()

    # Assert
    hal.deactivate_pump.assert_not_called()
    assert call("Lubrication cycle complete") not in logger.info.call_args_list


def test_pump_enters_error_state_if_pressure_not_reached_in_time():
    # Arrange
    hal = create_autospec(HalInterface, spec_set=True)
    command = create_autospec(CommandInterface, spec_set=True)
    logger = create_autospec(Logger, spec_set=True)

    hal.is_machine_on = True
    hal.is_pressure_ok = False  # geen druk bereikt

    ini = make_ini()

    timer = create_autospec(LubricationTimer, spec_set=True)
    timer.should_lubricate.return_value = False

    controller = make_controller(
        hal=hal,
        command=command,
        logger=logger,
        ini=ini,
        timer=timer,
    )

    # Start cyclus op t=12:00:00 zonder drukopbouw
    with freeze_time("2024-01-01 12:00:00"):
        controller.state.start_cycle(time.time())

    # Na 61s moet een fout optreden
    with freeze_time("2024-01-01 12:01:01"):
        controller.update()

    # Assert
    hal.deactivate_pump.assert_called_once()
    hal.activate_error.assert_called_once()
    command.error_msg.assert_called_once_with("Lubrication pressure not reached within 60 seconds!")
    logger.error.assert_called_once_with("Lubrication pressure not reached within 60 seconds")


def test_pump_does_not_enter_error_state_before_pressure_timeout():
    # Arrange
    hal = create_autospec(HalInterface, spec_set=True)
    command = create_autospec(CommandInterface, spec_set=True)
    logger = create_autospec(Logger, spec_set=True)

    hal.is_machine_on = True
    hal.is_pressure_ok = False  # nog steeds geen druk

    ini = make_ini()
    ini.pressure_timeout = 60

    timer = create_autospec(LubricationTimer, spec_set=True)
    timer.should_lubricate.return_value = False

    controller = make_controller(
        hal=hal,
        command=command,
        logger=logger,
        ini=ini,
        timer=timer,
    )

    # Start cyclus op t=12:00:00
    with freeze_time("2024-01-01 12:00:00"):
        controller.state.start_cycle(time.time())

    # Net voor timeout: t=12:00:59
    with freeze_time("2024-01-01 12:00:59"):
        controller.update()

    # Assert
    hal.deactivate_pump.assert_not_called()
    hal.activate_error.assert_not_called()
    command.error_msg.assert_not_called()
    assert all("pressure not reached" not in call.args[0] for call in logger.error.call_args_list)
