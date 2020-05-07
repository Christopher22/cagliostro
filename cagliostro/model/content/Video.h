/*
This file is part of cagliostro.
Copyright (C) 2020 Christopher Gundler <christopher@gundler.de>

This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>.
*/

#ifndef CAGLIOSTRO_CAGLIOSTRO_MODEL_CONTENT_VIDEO_H_
#define CAGLIOSTRO_CAGLIOSTRO_MODEL_CONTENT_VIDEO_H_

#include "Content.h"
#include <QThread>
#include <QAbstractVideoSurface>

namespace cagliostro::model::content {
namespace util {
class VideoDecoder;
}

class Video : public Content {
 Q_OBJECT
  Q_PROPERTY(QSize size READ size)

 public:
  static Video *load(const QUrl &uri, QObject *parent = nullptr);
  bool bind(QAbstractVideoSurface *output);
  bool show() override;
  void hide() override;
  [[nodiscard]] QSize size() const noexcept;

 protected:
  explicit Video(util::VideoDecoder *decoder, const QUrl &uri, QObject *parent = nullptr) noexcept;

 private:
  util::VideoDecoder *decoder_;
  QThread *worker_;
};
}

#endif //CAGLIOSTRO_CAGLIOSTRO_MODEL_CONTENT_VIDEO_H_
