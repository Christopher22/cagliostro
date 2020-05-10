/*
This file is part of cagliostro.
Copyright (C) 2020 Christopher Gundler <christopher@gundler.de>

This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>.
*/

#include "Scale.h"

#include <QGridLayout>
#include <QButtonGroup>
#include <QRadioButton>
#include <QLabel>

namespace cagliostro::view {

Scale::Scale(model::Selection *selection, QWidget *parent) : QWidget(parent), selection_(selection) {
  assert(selection != nullptr);
  QObject::connect(selection, &model::Selection::answered, this, &Scale::valueSelected);

  // Create the button group managing the selection
  auto *buttons = new QButtonGroup(this);
  QObject::connect(buttons, QOverload<int>::of(&QButtonGroup::buttonClicked), this, [&](int id) {
	selection_->setAnswer(id);
  });

  // Create the layout and fill it both with labels and buttons
  auto *layout = new QGridLayout(this);
  const auto values = (*selection_)->stringList();
  for (int i = 0, size = values.size(); i < size; ++i) {
	auto *button = new QRadioButton(this);
	auto *label = new QLabel(values[i], this);
	layout->addWidget(button, 0, i, Qt::AlignCenter);
	layout->addWidget(label, 1, i, Qt::AlignCenter);
	buttons->addButton(button, i);
	label->setBuddy(button);
  }
  this->setLayout(layout);
}

QString Scale::selection() const noexcept {
  return selection_->answer();
}

}