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

#include <QObject>
#include <QFile>
#include <QTextStream>

namespace cagliostro::model {
class Wizard : public QObject {
 Q_OBJECT
  Q_PROPERTY(QString participant READ participant)

 public:
  static Wizard *load(QString participant,
					  const QString &output_path,
					  const QString &title = tr("Cagliostro"),
					  QObject *parent = nullptr);
  [[nodiscard]] QVector<Page *> pages() noexcept;
  [[nodiscard]] QString participant() const noexcept;

  bool save(Question *question);

 protected:
  explicit Wizard(QString participant, QFile *file, const QString &title = tr("Cagliostro"), QObject *parent = nullptr);

 private:
  QString participant_;
  QTextStream output_;
};
}

#endif //CAGLIOSTRO_CAGLIOSTRO_MODEL_WIZARD_H_
