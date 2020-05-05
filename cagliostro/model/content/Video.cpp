/*
This file is part of cagliostro.
Copyright (C) 2020 Christopher Gundler <christopher@gundler.de>

This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>.
*/

#include "Video.h"

#include <QMediaPlayer>

namespace cagliostro::model::content {

Video::Video(QUrl uri, QObject *parent) noexcept: Content(std::move(uri), parent), media_(new QMediaPlayer(this)) {
  QObject::connect(media_, &QMediaPlayer::mediaStatusChanged, this, [this](QMediaPlayer::MediaStatus status) {
    if (this->status() == Status::Loading && status == QMediaPlayer::LoadedMedia) {
      this->setStatus(Status::Ready);
    }
  });

  QObject::connect(media_,
                   qOverload<QMediaPlayer::Error>(&QMediaPlayer::error),
                   this,
                   [this](QMediaPlayer::Error status) {
                     this->setStatus(Status::Failed);
                   });

}

void Video::bind(QVideoWidget *output) {
  media_->setVideoOutput(output);
}

void Video::load() {
  media_->setMedia(this->uri());
}

bool Video::show() {
  if (this->status() != Status::Ready) {
    return false;
  }
  media_->play();
  return true;
}
}