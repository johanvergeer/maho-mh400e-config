from typing import Any

import pytest

from lubrication.utils import strtobool


@pytest.mark.parametrize(
    "input_str,expected",
    [
        ("y", True),
        ("Y", True),
        (" yes ", True),
        ("T", True),
        ("true", True),
        (" on ", True),
        ("1", True),
        ("n", False),
        ("N", False),
        (" no ", False),
        ("F", False),
        ("false", False),
        (" off ", False),
        ("0", False),
    ],
)
def test_strtobool_valid_cases(input_str: str, expected: bool) -> None:
    assert strtobool(input_str) == expected


@pytest.mark.parametrize(
    "invalid_input", ["maybe", "2", "", "ok", "null", "None", "Truee", "falze", "10", "01"]
)
def test_strtobool_invalid_strings(invalid_input: str) -> None:
    with pytest.raises(ValueError):
        strtobool(invalid_input)


@pytest.mark.parametrize("non_string_input", [None, True, False, 0, 1, [], {}, 3.14, object()])
def test_strtobool_type_errors(non_string_input: Any) -> None:
    with pytest.raises(TypeError):
        strtobool(non_string_input)
