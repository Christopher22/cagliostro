/*
This file is part of cagliostro.
Copyright (C) 2020 Christopher Gundler <christopher@gundler.de>

This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>.
*/

#ifndef CAGLIOSTRO_CAGLIOSTRO_MODEL_SELECTION_H_
#define CAGLIOSTRO_CAGLIOSTRO_MODEL_SELECTION_H_

#include "Question.h"

#include <QStringListModel>
#include <QStringList>

namespace cagliostro::model {
class Selection : public Question {
 Q_OBJECT

 public:
  Selection(QString text, const QStringList &choices, const QString &name, int index = 0, QObject *parent = nullptr);
  QStringListModel *operator->() noexcept;

  [[nodiscard]] bool isAnswered() const noexcept override;
  [[nodiscard]] QString answer() const noexcept override;
  bool setAnswer(const QString &answer) override;
  bool setAnswer(int answer_index);

 private:
  QStringListModel *choices_;
  QPersistentModelIndex answer_;
};
}

#endif //CAGLIOSTRO_CAGLIOSTRO_MODEL_SELECTION_H_
