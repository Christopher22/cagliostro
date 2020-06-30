/*
This file is part of cagliostro.
Copyright (C) 2020 Christopher Gundler <christopher@gundler.de>

This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>.
*/

#include "VideoSurface.h"

namespace cagliostro::view::util {
VideoSurface::VideoSurface(VideoViewer *parent) : QAbstractVideoSurface(parent), viewer_(parent) {}

bool VideoSurface::present(const QVideoFrame &frame) {
  auto texture = viewer_->frame();
  if (texture == nullptr || !frame.isMapped() || !frame.isValid()) {
	return false;
  }

  texture->setData(QOpenGLTexture::BGR, QOpenGLTexture::UInt8, frame.bits());
  emit this->newFrame();
  return true;
}

QList<QVideoFrame::PixelFormat> VideoSurface::supportedPixelFormats(QAbstractVideoBuffer::HandleType handleType) const {
  return {QVideoFrame::Format_BGR24};
}

bool VideoSurface::start(const QVideoSurfaceFormat &format) {
  return QAbstractVideoSurface::start(format);
}

bool VideoSurface::isFormatSupported(const QVideoSurfaceFormat &format) const {
  return QAbstractVideoSurface::isFormatSupported(format);
}
}