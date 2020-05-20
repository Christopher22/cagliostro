/*
This file is part of cagliostro.
Copyright (C) 2020 Christopher Gundler <christopher@gundler.de>

This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>.
*/

#ifndef CAGLIOSTRO_CAGLIOSTRO_MODEL_PAGE_H_
#define CAGLIOSTRO_CAGLIOSTRO_MODEL_PAGE_H_

#include "Entity.h"
#include "Question.h"
#include "content/Content.h"

#include <QVector>

namespace cagliostro::model {
class Page : public Entity {
 Q_OBJECT

 public:
  explicit Page(const QString &name, int index = 0, QObject *parent = nullptr);
  [[nodiscard]] QVector<Question *> questions() noexcept;
  [[nodiscard]] content::Content *content() noexcept;

  [[nodiscard]] QString title() const noexcept;
  void setTitle(const QString &title) noexcept;
  [[nodiscard]] QString description() const noexcept;
  void setDescription(const QString &description) noexcept;
  [[nodiscard]] QString nextText() const noexcept;
  void setNextText(const QString &next_button_text) noexcept;

  bool save();

 signals:
  void saved();

 private:
  QString title_, description_, next_text_;
};
}

#endif //CAGLIOSTRO_CAGLIOSTRO_MODEL_PAGE_H_
