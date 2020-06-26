/*
This file is part of cagliostro.
Copyright (C) 2020 Christopher Gundler <christopher@gundler.de>

This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>.
*/

#include "VideoViewer.h"

#include <QPainter>
#include <QPaintEvent>

namespace cagliostro::view {

VideoViewer::VideoViewer(const QSize &size, QWidget *parent)
    : QLabel(parent), surface_(new VideoSurface(this)), frame_size_(size) {
  this->setScaledContents(true);
  this->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
}

QAbstractVideoSurface *VideoViewer::surface() {
  return surface_;
}

void VideoViewer::setFrame(const QImage &frame) {
  frame_size_ = frame.size();
  this->updateMargins();
  QLabel::setPixmap(QPixmap::fromImage(frame, Qt::ColorOnly));
}

void VideoViewer::updateMargins() {
  if (frame_size_.isEmpty()) {
    return;
  }

  const auto size = this->size();
  if (size.isEmpty()) {
    return;
  }

  if (size.width() * frame_size_.height() > size.height() * frame_size_.width()) {
    const int m = (size.width() - (frame_size_.width() * size.height() / frame_size_.height())) / 2;
    this->setContentsMargins(m, 0, m, 0);
  } else {
    const int m = (size.height() - (frame_size_.height() * size.width() / frame_size_.width())) / 2;
    this->setContentsMargins(0, m, 0, m);
  }
}

void VideoViewer::resizeEvent(QResizeEvent *event) {
  this->updateMargins();
  QLabel::resizeEvent(event);
}

VideoViewer::VideoSurface::VideoSurface(VideoViewer *parent) : QAbstractVideoSurface(parent) {}

bool VideoViewer::VideoSurface::present(const QVideoFrame &frame) {
  if (!frame.isMapped() || !frame.isValid()) {
    return false;
  }

  const uchar *bytes = frame.bits();
  const int width = frame.width(), height = frame.height(), bytes_per_line = frame.bytesPerLine();
  const QImage::Format format = QImage::Format_BGR888;
  qobject_cast<VideoViewer *>(this->parent())->setFrame(QImage(
      bytes,
      width,
      height,
      bytes_per_line,
      format
  ));
  return true;
}

QList<QVideoFrame::PixelFormat> VideoViewer::VideoSurface::supportedPixelFormats(QAbstractVideoBuffer::HandleType handleType) const {
  return {QVideoFrame::Format_BGR24};
}

bool VideoViewer::VideoSurface::start(const QVideoSurfaceFormat &format) {
  return QAbstractVideoSurface::start(format);
}

bool VideoViewer::VideoSurface::isFormatSupported(const QVideoSurfaceFormat &format) const {
  return QAbstractVideoSurface::isFormatSupported(format);
}

}