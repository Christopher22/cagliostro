"""
This file is part of cagliostro.
Copyright (C) 2020 Christopher Gundler <christopher@gundler.de>

This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>.
"""

from pathlib import Path
from dataclasses import dataclass
from typing import Iterable


@dataclass
class CagliostroFile:
    file: Path
    name: str
    password: str

    @staticmethod
    def find(parent_directory: str) -> Iterable["CagliostroFile"]:
        parent_directory = Path(parent_directory)
        for cagliostro_file in parent_directory.glob('*.cagliostro'):
            file_parts = cagliostro_file.stem.split('_')
            if len(file_parts) != 2:
                continue
            yield CagliostroFile(file=cagliostro_file, name=file_parts[0], password=file_parts[1])

    def __str__(self) -> str:
        return f"{self.name}:{self.password}"


if __name__ == '__main__':
    import argparse

    args = argparse.ArgumentParser('extract_passwords',
                                   description='Extract the passwords from filenames and rename them')
    args.add_argument('path', help='Path to the folder containing the files')
    args = args.parse_args()

    for file in CagliostroFile.find(args.path):
        print(file)
        file.file.rename(file.file.parent / f"{file.name}.cagliostro")
