import ctypes
import pathlib

SRC_DIR = pathlib.Path(__file__).parents[2] / "src" / "lubrication"

lib = ctypes.CDLL(str((SRC_DIR / "lubrication_logic.so").absolute()))  # of compile on the fly
lib.lubricate.argtypes = [ctypes.c_float]
lib.lubricate.restype = ctypes.c_float

def test_lubricate():
    assert lib.lubricate(3.0) == 7.0
