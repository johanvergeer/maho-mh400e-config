import os

import hal
import linuxcnc

from lubrication.utils import strtobool


class HalAdapter:
    def __init__(self) -> None:
        self.halcomp = hal.component("lube_pump")
        self.halcomp.newpin("machine_on", hal.HAL_BIT, hal.HAL_IN)
        self.halcomp.newpin("pressure_ok", hal.HAL_BIT, hal.HAL_IN)
        self.halcomp.newpin("pump_active", hal.HAL_BIT, hal.HAL_OUT)
        self.halcomp.newpin("error_active", hal.HAL_BIT, hal.HAL_OUT)
        self.halcomp.ready()

    @property
    def is_machine_on(self) -> bool:
        return self.halcomp["machine_on"]

    @property
    def is_pressure_ok(self) -> bool:
        return self.halcomp["pressure_ok"]

    def activate_pump(self) -> None:
        self.halcomp["pump_active"] = True

    def deactivate_pump(self) -> None:
        self.halcomp["pump_active"] = False

    def activate_error(self) -> None:
        self.halcomp["error_active"] = True

    def deactivate_error(self) -> None:
        self.halcomp["error_active"] = False


class StatAdapter:
    def __init__(self) -> None:
        self.stat = linuxcnc.stat()

    def poll(self) -> None:
        self.stat.poll()


class CommandAdapter:
    def __init__(self) -> None:
        self.command = linuxcnc.command()

    def text_msg(self, message: str) -> None:
        self.command.text_msg(message)

    def error_msg(self, message: str) -> None:
        self.command.error_msg(message)


class IniAdapter:
    def __init__(self) -> None:
        self.inifile = linuxcnc.ini(self._get_inifile_path())
        self._inifile_section = "LUBRICATION"

    def _get_inifile_path(self) -> str:
        inifile_path = os.environ.get("INI_FILE_NAME")
        if not inifile_path:
            raise RuntimeError("INI_FILE_NAME environment variable not set")
        return inifile_path

    @property
    def is_lubrication_enabled(self) -> bool:
        return bool(strtobool(self.inifile.find(self._inifile_section, "ENABLED") or "false"))

    @property
    def pump_interval(self) -> int:
        return int(self.inifile.find(self._inifile_section, "PUMP_INTERVAL_1"))

    @property
    def pressure_timeout(self) -> int:
        return int(self.inifile.find(self._inifile_section, "PRESSURE_TIMEOUT"))

    @property
    def pressure_hold_time(self) -> int:
        return int(self.inifile.find(self._inifile_section, "PRESSURE_HOLD_TIME"))
