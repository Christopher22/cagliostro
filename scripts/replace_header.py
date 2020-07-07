"""
This file is part of cagliostro.
Copyright (C) 2020 Christopher Gundler <christopher@gundler.de>

This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>.
"""

import argparse
from pathlib import Path


def change_header(file: Path, replacement: str) -> str:
    """
    Replace the header of a file.

    >>> example_file = Path("example.tmp")
    >>> with example_file.open("w+", encoding="ascii") as f: f.write("abcdefg")
    7
    >>> change_header(example_file, "0123")
    '30313233>61626364'
    >>> with example_file.open("r", encoding="ascii") as f: print(f.read())
    0123efg


    :param file: The path to the file.
    :param replacement: The replacement as string. The bytes, once transformed to ASCII, are utilized.
    :return: A replacement definition suitable for cagliostro's "header" attribute.
    """

    if not file.is_file():
        raise ValueError("Invalid file")

    new_header = replacement.encode(encoding="ascii", errors="throw")
    with file.open('r+b') as f:
        old_header = f.read(len(new_header))
        if len(old_header) != len(new_header):
            raise ValueError("Insufficient file size")

        f.seek(0)
        f.write(new_header)

    return f"{new_header.hex()}>{old_header.hex()}"


if __name__ == "__main__":
    args = argparse.ArgumentParser("header")
    args.add_argument("file", type=str, help="The target file")
    args.add_argument("header", type=str, help="The value as ASCII string for changing the header")
    args = args.parse_args()

    try:
        value = change_header(Path(args.file), args.header)
        print(value)
    except ValueError as ex:
        print(ex)
        exit(1)
