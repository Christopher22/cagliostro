//
// Created by Christopher Gundler on 16.05.2020.
//

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
  if (status_ != Status::Uninitialized && status_ != Status::Opening) {
    return false;
  }
  uri_ = std::move(uri);
  return true;
}

bool Resource::isOpened() const noexcept {
  return status_ == Status::Open && uri_.isValid();
}

}