/*
This file is part of cagliostro.
Copyright (C) 2020 Christopher Gundler <christopher@gundler.de>

This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>.
*/

#include "Header.h"

#include <QFile>

namespace cagliostro::model::content {

Header::Header(QByteArray before, QByteArray after, Resource *resource)
    : QObject(resource), before_(std::move(before)), after_(std::move(after)) {
  assert(resource != nullptr);
  assert(before_.size() == after_.size());

  QObject::connect(resource, &Resource::prepare, this, [this](Resource *res) {
    this->changeHeader(res, true);
  });
  QObject::connect(resource, &Resource::cleanUp, this, [this](Resource *res) {
    this->changeHeader(res, false);
  });
}

void Header::changeHeader(Resource *resource, bool is_preparing) {
  auto uri = resource->uri();
  if (!uri.isLocalFile()) {
    return;
  }

  QFile file(uri.toLocalFile());
  if (!file.open(QIODevice::ReadWrite)) {
    qWarning("Unable to open file for preparing header");
  }

  // Read the file header
  auto file_header = file.read(before_.size());
  file.seek(0);

  if (is_preparing && file_header == before_) {
    file.write(after_);
  } else if (!is_preparing && file_header == after_) {
    file.write(before_);
  }
}

Header *Header::create(const QString &header_info, Resource *resource) {
  if (resource == nullptr) {
    return nullptr;
  }

  // Try to separate the two parts
  const auto parts = header_info.toLatin1().split('>');
  if (parts.size() != 2) {
    return nullptr;
  }

  // Ensure that the data is valid
  const auto before = QByteArray::fromHex(parts[0]), after = QByteArray::fromHex(parts[1]);
  if (before.size() == 0 || before.size() != after.size()) {
    return nullptr;
  }

  return new Header(before, after, resource);
}
}