import os

import hal
import linuxcnc

from lubrication.adapters.interfaces import (
    CommandInterface,
    HalInterface,
    IniInterface,
    StatInterface,
)
from lubrication.utils import strtobool


class HalAdapter(HalInterface):
    def __init__(self) -> None:
        self.halcomp = hal.component("lube_pump")
        self.halcomp.newpin("machine_on", hal.HAL_BIT, hal.HAL_IN)
        self.halcomp.newpin("pressure_ok", hal.HAL_BIT, hal.HAL_IN)
        self.halcomp.newpin("pump_active", hal.HAL_BIT, hal.HAL_OUT)
        self.halcomp.newpin("error_active", hal.HAL_BIT, hal.HAL_OUT)
        self.halcomp.newpin("x_axis_position", hal.HAL_FLOAT, hal.HAL_IN)
        self.halcomp.newpin("y_axis_position", hal.HAL_FLOAT, hal.HAL_IN)
        self.halcomp.newpin("z_axis_position", hal.HAL_FLOAT, hal.HAL_IN)
        self.halcomp.ready()

    @property
    def is_machine_on(self) -> bool:
        return self.halcomp["machine_on"]

    @property
    def is_pressure_ok(self) -> bool:
        return self.halcomp["pressure_ok"]

    @property
    def x_axis_position(self) -> float:
        return self.halcomp["x_axis_position"]

    @property
    def y_axis_position(self) -> float:
        return self.halcomp["x_axis_position"]

    @property
    def z_axis_position(self) -> float:
        return self.halcomp["x_axis_position"]

    def activate_pump(self) -> None:
        self.halcomp["pump_active"] = True

    def deactivate_pump(self) -> None:
        self.halcomp["pump_active"] = False

    def activate_error(self) -> None:
        self.halcomp["error_active"] = True

    def deactivate_error(self) -> None:
        self.halcomp["error_active"] = False


class StatAdapter(StatInterface):
    def __init__(self) -> None:
        self.stat = linuxcnc.stat()

    def poll(self) -> None:
        self.stat.poll()


class CommandAdapter(CommandInterface):
    def __init__(self) -> None:
        self.command = linuxcnc.command()

    def text_msg(self, message: str) -> None:
        self.command.text_msg(message)

    def error_msg(self, message: str) -> None:
        self.command.error_msg(message)


class IniAdapter(IniInterface):
    def __init__(self) -> None:
        self.inifile = linuxcnc.ini(self._get_inifile_path())
        self._inifile_section = "LUBRICATION"

    def _get_inifile_path(self) -> str:
        inifile_path = os.environ.get("INI_FILE_NAME")
        if not inifile_path:
            raise RuntimeError("INI_FILE_NAME environment variable not set")
        return inifile_path

    def _get_float_value_or_default(self, name: str, default: float) -> float:
        if value := self.inifile.find(self._inifile_section, name):
            return float(value)
        return default

    def _get_int_value_or_default(self, name: str, default: int) -> int:
        if value := self.inifile.find(self._inifile_section, name):
            return int(value)
        return default


    @property
    def update_interval(self) -> float:
        return self._get_float_value_or_default("UPDATE_INTERVAL", 0.1)

    @property
    def is_lubrication_enabled(self) -> bool:
        return bool(strtobool(self.inifile.find(self._inifile_section, "ENABLED") or "false"))

    @property
    def pressure_timeout(self) -> int:
        return int(self.inifile.find(self._inifile_section, "PRESSURE_TIMEOUT"))

    @property
    def pressure_hold_time(self) -> int:
        return int(self.inifile.find(self._inifile_section, "PRESSURE_HOLD_TIME"))

    @property
    def movement_threshold(self) -> float:
        return self._get_float_value_or_default("MOVEMENT_THRESHOLD", 0.1)

    @property
    def movement_window_seconds(self) -> float:
        return self._get_float_value_or_default("MOVEMENT_WINDOW_SECONDS", 1)

    @property
    def interval_consecutive_movement(self) -> int:
        return self._get_int_value_or_default("INTERVAL_CONSECUTIVE_MOVEMENT", 16 * 60)
