/*
This file is part of cagliostro.
Copyright (C) 2020 Christopher Gundler <christopher@gundler.de>

This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>.
*/

#include "Question.h"

#include <utility>

namespace cagliostro::model {

Question::Question(QString text, const QString &name, int index, QObject *parent) :
    Entity(name, index, parent),
    text_(std::move(text)) {
  this->setObjectName(name);
}

void Question::setText(const QString &text) {
  text_ = text;
}

QString Question::text() const noexcept {
  return text_;
}

}