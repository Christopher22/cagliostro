/*
This file is part of cagliostro.
Copyright (C) 2020 Christopher Gundler <christopher@gundler.de>

This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>.
*/

#ifndef CAGLIOSTRO_CAGLIOSTRO_MODEL_QUESTION_H_
#define CAGLIOSTRO_CAGLIOSTRO_MODEL_QUESTION_H_

#include <QStringListModel>

#include "Entity.h"

namespace cagliostro::model {
class Question : public Entity {
 Q_OBJECT

 public:
  void setText(const QString &text);
  [[nodiscard]] QString text() const noexcept;

 protected:
  Question(QString text, const QString &name, int index = 0, QObject *parent = nullptr);

 private:
  QString text_;
};
}

#endif //CAGLIOSTRO_CAGLIOSTRO_MODEL_QUESTION_H_
