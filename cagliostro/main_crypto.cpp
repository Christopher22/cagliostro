/*
This file is part of cagliostro.
Copyright (C) 2020 Christopher Gundler <christopher@gundler.de>

This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>.
*/

#include "model/Source.h"

#include <QCoreApplication>
#include <QFile>
#include <QDebug>
#include <QFileInfo>

using namespace cagliostro;

enum class Status : int {
  Success = 0,
  InvalidArguments = 1,
  InputInvalid = 2,
  OutputInvalid = 3,
  EncryptionError = 4
};

int report(Status status) {
  switch (status) {
    case Status::InvalidArguments:
      qCritical()
          << QString("Usage: %1 INPUT_FILE OUTPUT_FILE PASSWORD").arg(QFileInfo(QCoreApplication::applicationFilePath()).fileName());
      break;
    case Status::InputInvalid:qCritical("Unable to open input file");
      break;
    case Status::OutputInvalid:qCritical("Unable to open output file");
      break;
    case Status::EncryptionError:qCritical("Unable to encrypt data");
      break;
    default:break;
  }
  return static_cast<int>(status);
}

int main(int argc, char *argv[]) {
  QCoreApplication app(argc, argv);
  if (QCoreApplication::arguments().size() != 4) {
    return report(Status::InvalidArguments);
  }

  // Try to open input and output files
  QFile input(QCoreApplication::arguments().at(1)), output(QCoreApplication::arguments().at(2));
  if (!input.exists() || !input.open(QIODevice::ReadOnly)) {
    return report(Status::InputInvalid);
  } else if (output.exists() || !output.open(QIODevice::WriteOnly)) {
    return report(Status::OutputInvalid);
  }

  // Encrypt the input file
  const auto data = model::Source::encrypt(&input, QCoreApplication::arguments().at(3));
  if (data.isEmpty() || output.write(data) != data.size()) {
    return report(Status::OutputInvalid);;
  }

  return report(Status::Success);
}