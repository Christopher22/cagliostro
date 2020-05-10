/*
This file is part of cagliostro.
Copyright (C) 2020 Christopher Gundler <christopher@gundler.de>

This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>.
*/

#include "model/Config.h"
#include "view/Wizard.h"

#include <QApplication>

#include <iostream>

using namespace cagliostro;

enum class ReturnCode : int {
  Success = 0,
  MissingArgument = 1,
  InvalidFile = 2,
  ParserError = 3
};

int main(int argc, char *argv[]) {
  QApplication app(argc, argv);

  // Set some meta data
  QApplication::setOrganizationName("Christopher Gundler");
  QApplication::setApplicationName("cagliostro");

  if (argc != 2) {
	std::cout << QString("Usage: %1 cagliostro_file").arg(argc == 1 ? argv[0] : "cagliostro").toUtf8().constData()
			  << std::endl;
	return static_cast<int>(ReturnCode::MissingArgument);
  }

  QScopedPointer parser(new model::Config());
  QObject::connect(parser.get(), &model::Config::done, [&](model::Wizard *wizard) {
	wizard->setParent(parser.get());
	(new view::Wizard(wizard))->show();
  });
  QObject::connect(parser.get(), &model::Config::error, [](model::Config::Error, const QString &error) {
	std::cerr << "Parsing error: " << error.toUtf8().constData() << std::endl;
  });

  // Get info of the file given as argument
  const QFileInfo file_info(argv[1]);
  QFile file(file_info.absoluteFilePath());
  if (!file.open(QIODevice::ReadOnly)) {
	return static_cast<int>(ReturnCode::InvalidFile);
  }

  // Parse the file and run
  if (parser->parse(&file, file_info.absolutePath())) {
	return QApplication::exec();
  } else {
	return static_cast<int>(ReturnCode::ParserError);
  }
}
