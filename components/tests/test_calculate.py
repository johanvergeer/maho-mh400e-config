import ctypes
import pathlib

SRC_DIR = pathlib.Path(__file__).parents[1] / "src"

lib = ctypes.CDLL(str((SRC_DIR / "calculate.so").absolute()))  # of compile on the fly
lib.calculate.argtypes = [ctypes.c_float]
lib.calculate.restype = ctypes.c_float

def test_calculate():
    assert lib.calculate(3.0) == 7.0
