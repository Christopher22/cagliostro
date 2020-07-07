import random
import string
import subprocess
import uuid
import xml.etree.ElementTree as Xml
from dataclasses import dataclass
from enum import IntEnum
from pathlib import Path

from shuffle_pages import shuffle_tree


class ResultCode(IntEnum):
    """
    An status code regarding the success of the operation.
    """
    Success = 0,
    IOError = 42,
    InvalidCagliostro = 43
    EncryptionFailed = 44

    def __bool__(self):
        return self.value == 0


@dataclass
class ConfigFile:
    """
    A wrapper class for an customized Config file
    """

    subject: str
    file: Path

    def encrypt(self, password: str, cryptor_path: Path) -> None:
        """
        Encrypts an existing config file and update the path.
        :param password: The used password.
        :param cryptor_path: The path to the Cagliostro crypto utility.
        """

        if cryptor_path.stem != "cagliostro_crypto" or not cryptor_path.is_file():
            raise ValueError("Invalid path to 'cagliostro_crypto'.")

        new_file = Path(self.file.parent, f"{self.subject}_{password}.cagliostro")
        subprocess.run(
            args=[str(cryptor_path), str(self.file.absolute()), str(new_file.absolute()), password],
            check=True
        )

        self.file.unlink()
        self.file = new_file

    @staticmethod
    def create(template: Xml, output_dir: Path, filter_include: str) -> "ConfigFile":
        """
        Create a new Cagliostro file from a parse template.
        :param template: A parsed Cagliostro XML element.
        :param output_dir: The directory where all the generated files are put in.
        :param filter_include: A filter expression shuffling only those pages where the id matches the pattern.
        :return: A wrapper object.
        """

        shuffled_template = shuffle_tree(template, filter_include=filter_include, seed=None)
        root = shuffled_template.getroot()

        subject_id = str(uuid.uuid4())[:4]
        root.attrib["participant"] = subject_id
        root.attrib["result"] = f"{subject_id}.result"

        output_file = Path(output_dir, f"{subject_id}.cagliostro")
        with output_file.open("wb+") as f:
            shuffled_template.write(f)

        return ConfigFile(subject=subject_id, file=output_file)


def create_batch(template: Path, crypto_path: Path, output: Path, num_subjects: int,
                 password_length: int, filter_include: str) -> ResultCode:
    """
    Create Cagliostro config files batchwise.
    :param template: The path to the Cagliostro file being a template.
    :param crypto_path: The path to the Cagliostro crypto utility.
    :param output: The directory where all the generated files are put in.
    :param num_subjects: The number of subjects.
    :param password_length: The length of the password.
    :param filter_include: A filter expression shuffling only those pages where the id matches the pattern.
    :return: A code describing if the operation was successful.
    """

    if not output.exists():
        output.mkdir(parents=True)

    # Create the subject files
    try:
        with template.open("rb") as f:
            template = Xml.parse(f)
            subjects = [ConfigFile.create(template, output, filter_include=filter_include) for _ in range(num_subjects)]
    except IOError:
        return ResultCode.IOError
    except Xml.ParseError:
        return ResultCode.InvalidCagliostro

    # Encrypt them
    try:
        for subject in subjects:
            password = ''.join(random.choices(string.ascii_lowercase, k=password_length))
            subject.encrypt(cryptor_path=crypto_path, password=password)
    except subprocess.CalledProcessError:
        return ResultCode.EncryptionFailed

    return ResultCode.Success


if __name__ == '__main__':
    from argparse import ArgumentParser

    parser = ArgumentParser(prog="batch_subject_creation.py",
                            description="This script creates multiple subject files")
    parser.add_argument("cagliostro_template", type=str, help="The un-encrypted cagliostro template")
    parser.add_argument("crypto_tool", type=str, help="Path to the compiled crypto tool")
    parser.add_argument("output_dir", type=str, help="The directory which will contain the generated files")
    parser.add_argument("--num_subjects", type=int, default=3, help="The number of subjects")
    parser.add_argument("--password_length", type=int, default=5, help="The length of the password")
    parser.add_argument("--include_pages", type=str, default="*",
                        help="Expression for shuffling only those pages where the id matches the pattern.")
    args = parser.parse_args()

    result = create_batch(
        template=Path(args.cagliostro_template),
        crypto_path=Path(args.crypto_tool),
        output=Path(args.output_dir),
        num_subjects=args.num_subjects,
        password_length=args.password_length,
        filter_include=args.include_pages
    )
    if not result:
        parser.exit(result.value)
