"""
This file is part of cagliostro.
Copyright (C) 2020 Christopher Gundler <christopher@gundler.de>

This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>.
"""

import xml.etree.ElementTree as Xml
from argparse import ArgumentParser
from enum import IntEnum
from fnmatch import fnmatch
from pathlib import Path
from random import Random
from typing import Optional


class ResultCode(IntEnum):
    """
    An status code regarding the success of the operation.
    """
    Success = 0,
    InputInvalid = 1,
    OutputInvalid = 2

    def __bool__(self):
        return self.value == 0


def shuffle_pages(input_file: str, output_file: str, filter_include: str = "*",
                  seed: Optional[int] = None) -> ResultCode:
    """
    Shuffles the pages in a not encrypted Cagliostro file.

    >>> import xml.etree.ElementTree as Xml
    >>> input_file = Path("example_input.xml")
    >>> output_file = Path("example_output.xml")
    >>> input_file.write_text('<cagliostro><page id="p1" /><page id="p2" /><page id="fixed" /><page id="p3" /></cagliostro>')
    92
    >>> bool(shuffle_pages(str(input_file), str(output_file), seed=80, filter_include="p*"))
    True
    >>> [node.attrib["id"] for node in Xml.parse(output_file).getroot().findall("*")]
    ['p1', 'p3', 'fixed', 'p2']

    :param input_file: The path to the Cagliostro file.
    :param output_file: The non-existing output Cagliostro file.
    :param filter_include: A filter expression shuffling only those pages where the id matches the pattern.
    :param seed: An optional seed for the random generator.
    :return: An status code.
    """

    input_file = Path(input_file)
    output_file = Path(output_file)
    if not input_file.exists():
        return ResultCode.InputInvalid
    elif output_file.exists():
        return ResultCode.OutputInvalid

    with input_file.open("r") as input_file, output_file.open("wb+") as output_file:
        output = Xml.parse(input_file)
        input_file.seek(0)
        input_root = Xml.parse(input_file).getroot()
        output_root = output.getroot()
        if input_root.tag != "cagliostro":
            return ResultCode.InputInvalid

        indices = [
            i for i, child in enumerate(input_root.findall('*'))
            if child.tag == "page" and fnmatch(child.attrib.get("id", ""), filter_include)
        ]
        new_indices = indices[:]

        rng = Random(seed)
        rng.shuffle(new_indices)
        for old, new in zip(indices, new_indices):
            output_root[new] = input_root[old]

        output.write(output_file)
        return ResultCode.Success


if __name__ == "__main__":
    parser = ArgumentParser(prog="shuffle_pages.py",
                            description="This script shuffles the pages in a calgiostro file randomly")
    parser.add_argument("input", type=str, help="The un-encrypted cagliostro file")
    parser.add_argument("output", type=str, help="The generated output file")
    parser.add_argument("--include_pages", type=str, default="*",
                        help="Expression for shuffling only those pages where the id matches the pattern.")

    args = parser.parse_args()
    result = shuffle_pages(args.input, args.output, filter_include=args.include_pages)
    if not result:
        parser.exit(result.value)
