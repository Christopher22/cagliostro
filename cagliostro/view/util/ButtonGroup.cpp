/*
This file is part of cagliostro.
Copyright (C) 2020 Christopher Gundler <christopher@gundler.de>

This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>.
*/

#include "ButtonGroup.h"

namespace cagliostro::view::util {

ButtonGroup::ButtonGroup(const QVector<QAbstractButton *> &default_buttons, QWidget *parent)
    : QWidget(parent),
      buttons_(new QButtonGroup(this)),
      visualization_(new QHBoxLayout()),
      reserved_(default_buttons.size()),
      current_id_(0) {

  for (auto button: default_buttons) {
    this->addButton(button);
  }
  this->setLayout(visualization_);
}

int ButtonGroup::addButton(QAbstractButton *button) noexcept {
  if (button == nullptr) {
    return -1;
  }

  const int id = current_id_++;
  buttons_->addButton(button, id);
  button->setParent(this);
  visualization_->insertWidget(0, button);
  return id;
}

bool ButtonGroup::removeButton(int id) noexcept {
  QAbstractButton *button = buttons_->button(id);
  if (id < reserved_ || button == nullptr) {
    return false;
  }

  visualization_->removeWidget(button);
  buttons_->removeButton(button);
  button->deleteLater();
  return true;
}

QAbstractButton *ButtonGroup::button(int id) noexcept {
  return buttons_->button(id);
}

}