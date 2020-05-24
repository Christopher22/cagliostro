/*
This file is part of cagliostro.
Copyright (C) 2020 Christopher Gundler <christopher@gundler.de>

This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>.
*/

#include "Video.h"
#include "util/VideoDecoder.h"

#include <QMediaPlayer>
#include <QVideoWidget>

namespace cagliostro::model::content {

Video *Video::load(Resource *resource, QObject *parent) {
  // Try to load the URL
  const auto uri = resource->uri();
  if (!uri.isValid()) {
    return nullptr;
  }

  // Try to decode the video
  auto *decoder = util::VideoDecoder::load(uri);
  if (decoder == nullptr) {
    return nullptr;
  }

  return new Video(decoder, resource, parent);
}

Video::Video(util::VideoDecoder *decoder, Resource *resource, QObject *parent) noexcept: Content(resource, parent),
                                                                                         decoder_(decoder),
                                                                                         worker_(new QThread(this)) {
  assert(decoder != nullptr);

  decoder_->moveToThread(worker_);
  QObject::connect(this, &QObject::destroyed, this, &Video::stopWorker);
  QObject::connect(decoder_, &util::VideoDecoder::done, this, &Video::stopWorker);
  QObject::connect(decoder_, &util::VideoDecoder::done, this, [this]() {
    this->setFinished();
  });
}

bool Video::bind(QAbstractVideoSurface *output) {
  assert(output != nullptr);
  decoder_->setVideoSurface(output);
  return true;
}

bool Video::show() {
  if (worker_->isRunning()) {
    return false;
  }

  worker_->start();
  return decoder_->start();
}

void Video::hide() {
  if (!worker_->isRunning()) {
    return;
  }
  decoder_->stop(true);
  this->stopWorker();

  // Close the resource
  Content::hide();
}

QSize Video::size() const noexcept {
  return decoder_->size();
}

void Video::stopWorker() {
  if (worker_->isRunning() && !worker_->isFinished()) {
    decoder_->stop(true);
    worker_->quit();
    if (!worker_->wait(1000)) {
      qWarning("Unable to close the decoder thread");
    }
  }
}

}