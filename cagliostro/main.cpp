/*
This file is part of cagliostro.
Copyright (C) 2020 Christopher Gundler <christopher@gundler.de>

This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>.
*/

#ifndef BUILD_TESTING

#include "model/Config.h"
#include "view/Wizard.h"

#include <QApplication>
#include <QTranslator>
#include <QSurfaceFormat>

#include <iostream>

using namespace cagliostro;

int main(int argc, char *argv[]) {
  // OpenGL config
  auto format = QSurfaceFormat::defaultFormat();
  format.setSwapInterval(0);
  QSurfaceFormat::setDefaultFormat(format);

  QApplication app(argc, argv);

  // Set some meta data
  QApplication::setOrganizationName("Christopher Gundler");
  QApplication::setApplicationName("cagliostro");
  QApplication::setQuitOnLastWindowClosed(true);

  // Try to load translations on runtime
  QTranslator translator;
  if (translator.load(QLocale(), QLatin1String("cagliostro"), QLatin1String("_"), QLatin1String("translations"))) {
	QCoreApplication::installTranslator(&translator);
  }

  // Run the GUI
  (new view::Wizard())->show();
  return QApplication::exec();
}

#else
#include "Tests.h"
QTEST_MAIN(Tests)
#endif
