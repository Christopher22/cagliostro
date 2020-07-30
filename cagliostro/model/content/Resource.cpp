/*
This file is part of cagliostro.
Copyright (C) 2020 Christopher Gundler <christopher@gundler.de>

This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>.
*/

#include "Resource.h"

#include <QFileInfo>

namespace cagliostro::model::content {

Resource::Resource(QUrl uri, QObject *parent) : QObject(parent), uri_(std::move(uri)), status_(Status::Uninitialized) {

}

Resource::Resource(const QString &path, const QDir &root_dir, QObject *parent)
    : QObject(parent), status_(Status::Uninitialized) {
  uri_ = QUrl(path);
  if (uri_.isLocalFile() || !path.contains("://")) {
    // Make files absolute regarding the root directory.
    QFileInfo info(!uri_.isLocalFile() ? path : uri_.toLocalFile());
    if (info.isRelative()) {
      info = QFileInfo(root_dir.filePath(info.filePath()));
    }

    // Ensure a local file exists
    uri_ = info.exists() ? QUrl::fromLocalFile(info.absoluteFilePath()) : QUrl();
  }
}

Resource::~Resource() {
  this->close();
}

QUrl Resource::uri() noexcept {
  if (status_ == Status::Uninitialized && uri_.isValid()) {
    status_ = Status::Opening;
    emit this->prepare(this);
    status_ = Status::Open;
  }
  return uri_;
}

void Resource::close() noexcept {
  if (status_ == Status::Closing || status_ == Status::Closed || !uri_.isValid()) {
    return;
  }

  status_ = Status::Closing;
  emit this->cleanUp(this);
  status_ = Status::Closed;
  uri_ = QUrl();
}

bool Resource::update(QUrl uri) {
  if (status_ != Status::Uninitialized && status_ != Status::Opening && status_ != Status::Closing) {
    return false;
  }
  uri_ = std::move(uri);
  return true;
}

bool Resource::isOpened() const noexcept {
  return status_ == Status::Open && uri_.isValid();
}

}