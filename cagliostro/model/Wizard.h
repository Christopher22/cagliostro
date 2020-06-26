/*
This file is part of cagliostro.
Copyright (C) 2020 Christopher Gundler <christopher@gundler.de>

This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>.
*/

#ifndef CAGLIOSTRO_CAGLIOSTRO_MODEL_WIZARD_H_
#define CAGLIOSTRO_CAGLIOSTRO_MODEL_WIZARD_H_

#include "Page.h"
#include "Question.h"
#include "Responses.h"

#include <QObject>
#include <QFile>
#include <QTextStream>

namespace cagliostro::model {
class Wizard : public QObject {
 Q_OBJECT
  Q_PROPERTY(QString participant READ participant)
  Q_PROPERTY(QString font_size READ participant)
 public:
  explicit Wizard(QString participant,
				  const QString &title,
				  QString no_questions_message,
				  int font_size,
				  QObject *parent = nullptr);
  [[nodiscard]] QVector<Page *> pages() const noexcept;
  [[nodiscard]] Responses *responses() noexcept;
  [[nodiscard]] QString participant() const noexcept;
  [[nodiscard]] QString completeMessage() const noexcept;
  [[nodiscard]] int fontSize() const noexcept;

  bool save(Page *page);
  [[nodiscard]] bool includeQuestions() const noexcept;

 private:
  QString participant_, complete_message_;
  int font_size_;
};
}

#endif //CAGLIOSTRO_CAGLIOSTRO_MODEL_WIZARD_H_
