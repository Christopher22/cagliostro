/*
This file is part of cagliostro.
Copyright (C) 2020 Christopher Gundler <christopher@gundler.de>

This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>.
*/

#ifndef CAGLIOSTRO_CAGLIOSTRO_MODEL_CONFIG_H_
#define CAGLIOSTRO_CAGLIOSTRO_MODEL_CONFIG_H_

#include "Wizard.h"
#include "content/Content.h"
#include "Selection.h"

#include <QDir>
#include <QIODevice>
#include <QXmlStreamReader>

#include <functional>

namespace cagliostro::model {

class Config : public QObject {
 Q_OBJECT

 public:
  enum class Error {
	ParserError,
	ResourceError,
	OutputError
  };

  explicit Config(QIODevice *data, const QDir &root_dir, QObject *parent = nullptr);
  void parse();
  explicit operator bool() const noexcept;

 signals:
  void done(Wizard *);
  void error(cagliostro::model::Config::Error, QString);

 private:
  Page *parse(Wizard *parent);
  Question *parse(Page *parent);
  bool parse(const char *name, std::function<void()> inner_element_values, bool include_root = true);

  QString attribute(const char *name, const QString &default_value = "");

  QDir root_;
  QXmlStreamReader xml_;
};

}

#endif //CAGLIOSTRO_CAGLIOSTRO_MODEL_CONFIG_H_
