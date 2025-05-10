def strtobool(val: str) -> bool:
    """Convert a string representation of truth to True or False.

    Accepted true values are: "y", "yes", "t", "true", "on", "1".
    Accepted false values are: "n", "no", "f", "false", "off", "0".

    The comparison is case-insensitive and ignores surrounding whitespace.

    Args:
        val: The string to convert.

    Returns:
        A boolean indicating the truth value of the string.

    Raises:
        TypeError: If the input is not a string.
        ValueError: If the string does not match any known true/false value.
    """
    if not isinstance(val, str):
        raise TypeError(f"invalid truth type {val!r}")
    val = val.lower().strip()
    if val in ("y", "yes", "t", "true", "on", "1"):
        return True
    elif val in ("n", "no", "f", "false", "off", "0"):
        return False
    else:
        raise ValueError(f"invalid truth value {val!r}")
