"""
This file is part of cagliostro.
Copyright (C) 2020 Christopher Gundler <christopher@gundler.de>

This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>.
"""

import argparse
from pathlib import Path

if __name__ == "__main__":
    args = argparse.ArgumentParser("header")
    args.add_argument("file", type=str, help="The target file")
    args.add_argument("header", type=str, help="The value to change the header to (in hexadecimal)")
    args = args.parse_args()

    file = Path(args.file)
    if not file.is_file():
        raise ValueError("Invalid file")
    elif len(args.header) < 2 or len(args.header) % 2 != 0:
        raise ValueError("Invalid Hex input")

    new_header = bytearray(int(args.header[i:i + 2], 8) for i in range(0, len(args.header), 2))
    with file.open('r+b') as f:
        old_header = f.read(len(new_header))
        if len(old_header) != len(new_header):
            raise ValueError("Insufficient file size")

        f.seek(0)
        f.write(new_header)

    print(f"{new_header.hex()}>{old_header.hex()}")
