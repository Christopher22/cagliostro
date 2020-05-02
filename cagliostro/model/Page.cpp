/*
This file is part of cagliostro.
Copyright (C) 2020 Christopher Gundler <christopher@gundler.de>

This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>.
*/

#include "Page.h"

namespace cagliostro::model {

Page::Page(const QString &name, int index, QObject *parent) : Entity(name, index, parent), next_text_(tr("Next")) {}

QVector<Question *> Page::questions() noexcept {
  return Entity::get<Question>(this);
}

content::Content *Page::content() noexcept {
  return this->findChild<content::Content *>(QString(), Qt::FindDirectChildrenOnly);
}

QString Page::title() const noexcept {
  return title_;
}

void Page::setTitle(const QString &title) noexcept {
  title_ = title;
}

QString Page::description() const noexcept {
  return description_;
}

void Page::setDescription(const QString &description) noexcept {
  description_ = description;
}

QString Page::nextText() const noexcept {
  return next_text_;
}

void Page::setNextText(const QString &next_button_text) noexcept {
  next_text_ = next_button_text;
}

}