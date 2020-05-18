/*
This file is part of cagliostro.
Copyright (C) 2020 Christopher Gundler <christopher@gundler.de>

This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>.
*/

#include "Content.h"

namespace cagliostro::model::content {

Content::Content(Resource *resource, QObject *parent) : QObject(parent) {
  assert(resource != nullptr);
  assert(resource->parent() == nullptr);
  resource->setParent(this);
}

void Content::hide() {
  auto *resource = this->resource();
  if (resource != nullptr) {
    resource->close();
  }
}

Resource *Content::resource() const {
  return this->findChild<Resource *>(QString(), Qt::FindDirectChildrenOnly);
}

}