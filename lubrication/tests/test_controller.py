from unittest.mock import MagicMock, create_autospec

import pytest

from lubrication.adapters.interfaces import (
    CommandInterface,
    HalInterface,
    IniInterface,
    Logger,
    StatInterface,
)
from lubrication.controller import LubePumpController, LubricationTimer


def test_controller_skips_lubrication_when_disabled():
    # Arrange
    hal = create_autospec(HalInterface)
    stat = create_autospec(StatInterface)
    command = create_autospec(CommandInterface)
    logger = create_autospec(Logger)

    ini = MagicMock(spec=IniInterface)
    ini.is_lubrication_enabled = False
    ini.pump_interval = 480
    ini.pressure_timeout = 60
    ini.pressure_hold_time = 15

    timer = LubricationTimer(interval_seconds=ini.pump_interval)

    controller = LubePumpController(
        hal_adapter=hal,
        stat_adapter=stat,
        command_adapter=command,
        ini_adapter=ini,
        logger=logger,
        timer=timer,
    )

    # Act
    controller.update()

    # Assert
    hal.activate_pump.assert_not_called()
    hal.deactivate_pump.assert_not_called()
    logger.info.assert_called_with("Initializing lubrication")
    logger.warning.assert_called_with("Lubrication logic is disabled via INI file")
    command.text_msg.assert_called_once()
