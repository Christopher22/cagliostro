"""
This file is part of cagliostro.
Copyright (C) 2020 Christopher Gundler <christopher@gundler.de>

This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>.
"""

import random
import string
import sys
import xml.etree.ElementTree as Xml
from argparse import ArgumentParser
from pathlib import Path

from replace_header import change_header


def change_all_headers(cagliostro_config: Path, length: int) -> None:
    """
    Change the first bytes of all the content in a Cagliostro file by a random sequence of length.
    :param cagliostro_config: The un-encrypted Cagliostro file.
    :param length: The length of the replacement.
    """

    with cagliostro_config.open("r", encoding="utf-8") as input_file:
        document = Xml.parse(input_file)
        for page in document.getroot().findall('page'):
            content = page.find('content')
            if content is None or ':' in content.text:
                continue

            # Extract path to file
            path = Path(content.text)
            if not path.exists() and not path.is_absolute():
                path = Path(cagliostro_config.parent, path)
            if not path.is_file():
                sys.stderr.write(f"Unable to open file '{content.text}'.\n")
                continue

            # Remove the extension of the file
            new_path = Path(path.parent, path.stem)
            path.rename(new_path)

            password = ''.join(random.choices(string.ascii_lowercase, k=length))
            content.attrib['header'] = change_header(new_path, password)
            content.text = str(new_path.relative_to(cagliostro_config.parent))

    # Write the changed file
    with cagliostro_config.open("wb+") as output_file:
        document.write(output_file)


if __name__ == "__main__":
    parser = ArgumentParser(prog="shuffle_pages.py",
                            description="This script changes the header of all the content mentioned.")
    parser.add_argument("cagliostro_file", type=str, help="The un-encrypted cagliostro file")
    parser.add_argument("--length", type=str, default=4,
                        help="The length of the replacement.")

    args = parser.parse_args()
    change_all_headers(Path(args.cagliostro_file), length=args.length)
