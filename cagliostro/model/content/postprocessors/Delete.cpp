/*
This file is part of cagliostro.
Copyright (C) 2020 Christopher Gundler <christopher@gundler.de>

This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>.
*/

#include "Delete.h"

#include <QFile>

namespace cagliostro::model::content {

Delete::Delete(Resource *parent) : QObject(parent) {
  QObject::connect(parent, &Resource::cleanUp, this, &Delete::cleanUp);
}

void Delete::cleanUp(Resource *resource) {
  const auto uri = resource->uri();
  if (!uri.isLocalFile()) {
    return;
  }

  auto file = uri.toLocalFile();
  if (QFile::remove(file)) {
    resource->update(QUrl());
  } else {
    qWarning("Unable to delete file");
  }
}
}