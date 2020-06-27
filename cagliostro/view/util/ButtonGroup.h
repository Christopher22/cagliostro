/*
This file is part of cagliostro.
Copyright (C) 2020 Christopher Gundler <christopher@gundler.de>

This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>.
*/

#ifndef CAGLIOSTRO_CAGLIOSTRO_VIEW_UTIL_BUTTONGROUP_H_
#define CAGLIOSTRO_CAGLIOSTRO_VIEW_UTIL_BUTTONGROUP_H_

#include <QWidget>
#include <QButtonGroup>
#include <QHBoxLayout>
#include <QAbstractButton>
#include <QVector>

namespace cagliostro::view::util {
class ButtonGroup : public QWidget {
 public:
  explicit ButtonGroup(const QVector<QAbstractButton *> &default_buttons, QWidget *parent = nullptr);

  int addButton(QAbstractButton *button) noexcept;
  bool removeButton(int id) noexcept;
  QAbstractButton *button(int id) noexcept;

 private:
  QButtonGroup *buttons_;
  QHBoxLayout *visualization_;
  int reserved_, current_id_;
};
}

#endif //CAGLIOSTRO_CAGLIOSTRO_VIEW_UTIL_BUTTONGROUP_H_
