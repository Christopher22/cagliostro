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
#include "Source.h"

#include <QDir>
#include <QIODevice>
#include <QXmlStreamReader>

#include <functional>

namespace cagliostro::model {

class Config : public QObject {
 Q_OBJECT

 public:
  enum class Error {
    InputError,
    ParserError,
    ResourceError,
    OutputError,
    FailedCheck
  };

  explicit Config(Source *data, const QDir &root_dir, QObject *parent = nullptr);
  void parse();
  explicit operator bool() const noexcept;

 signals:
  void done(Wizard *);
  void error(cagliostro::model::Config::Error, QString);

 private:
  Page *parse(Wizard *parent);
  Question *parse(Page *parent);
  bool parse(const char *name, std::function<void()> inner_element_values, bool include_root = true);

  template<typename T = QString>
  [[nodiscard]] T attribute(const char *name, T &&default_value = {}) const noexcept {
    static_assert(false, "This type is not supported");
  }

  template<>
  [[nodiscard]] inline QString attribute(const char *name, QString &&default_value) const noexcept {
	const QStringRef attribute_value = xml_.attributes().value(name);
	return attribute_value.isEmpty() && !default_value.isEmpty() ? default_value : attribute_value.toString();
  }

  template<>
  [[nodiscard]] inline bool attribute(const char *name, bool &&default_value) const noexcept {
	const auto raw_value = this->attribute<QString>(name, default_value ? "true" : "false").toLower();
	return raw_value == "true" || raw_value == "1";
  }

  template<>
  [[nodiscard]] inline int attribute(const char *name, int &&default_value) const noexcept {
	bool success = false;
	const auto raw_value = this->attribute<QString>(name, QString::number(default_value));
	const auto result = raw_value.toInt(&success);
	return success ? result : default_value;
  }

  Source *source_;
  QDir root_;
  QXmlStreamReader xml_;
};

}

#endif //CAGLIOSTRO_CAGLIOSTRO_MODEL_CONFIG_H_
