/*
This file is part of cagliostro.
Copyright (C) 2020 Christopher Gundler <christopher@gundler.de>

This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>.
*/

#ifndef CAGLIOSTRO_CAGLIOSTRO_VIEW_UTIL_VIDEOSURFACE_H_
#define CAGLIOSTRO_CAGLIOSTRO_VIEW_UTIL_VIDEOSURFACE_H_

#include "../VideoViewer.h"

#include <QAbstractVideoSurface>

namespace cagliostro::view::util {
class VideoSurface : public QAbstractVideoSurface {
 Q_OBJECT

 public:
  explicit VideoSurface(VideoViewer *parent);

  bool present(const QVideoFrame &frame) override;
  bool start(const QVideoSurfaceFormat &format) override;
  [[nodiscard]] QList<QVideoFrame::PixelFormat> supportedPixelFormats(QAbstractVideoBuffer::HandleType handleType) const override;
  [[nodiscard]] bool isFormatSupported(const QVideoSurfaceFormat &format) const override;

 signals:
  void newFrame();

 private:
  VideoViewer *viewer_;
};
}

#endif //CAGLIOSTRO_CAGLIOSTRO_VIEW_UTIL_VIDEOSURFACE_H_
