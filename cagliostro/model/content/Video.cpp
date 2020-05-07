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

Video *Video::load(const QUrl &uri, QObject *parent) {
  auto *decoder = util::VideoDecoder::load(uri);
  if (decoder == nullptr) {
	return nullptr;
  }
  return new Video(decoder, uri, parent);
}

Video::Video(util::VideoDecoder *decoder, const QUrl &uri, QObject *parent) noexcept: Content(uri, parent),
																					  decoder_(decoder),
																					  worker_(new QThread(this)) {
  assert(decoder != nullptr);

  decoder_->moveToThread(worker_);
  worker_->start();

  QObject::connect(this, &QObject::destroyed, decoder_, &util::VideoDecoder::deleteLater);
}

bool Video::bind(QAbstractVideoSurface *output) {
  assert(output != nullptr);
  decoder_->setVideoSurface(output);
  return true;
}

bool Video::show() {
  return decoder_->start();
}

void Video::hide() {
  decoder_->stop();
}

QSize Video::size() const noexcept {
  return decoder_->size();
}

}