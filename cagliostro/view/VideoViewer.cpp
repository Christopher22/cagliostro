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
	: QWidget(parent), surface_(new VideoSurface(this)), frame_(size, QImage::Format_BGR888) {
  this->setFixedSize(size);
  this->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
}

QAbstractVideoSurface *VideoViewer::surface() {
  return surface_;
}

void VideoViewer::paintEvent(QPaintEvent *event) {
  if (frame_.isNull()) {
	return;
  }
  QPainter painter(this);
  painter.drawImage(event->rect(), frame_, event->rect());
}

void VideoViewer::setFrame(const QImage &frame) {
  frame_ = frame;
  const auto frame_size = frame_.size();
  if (frame_size != this->size()) {
	this->setFixedSize(frame_size);
  }
  this->update();
}

QSize VideoViewer::sizeHint() const {
  return frame_.size();
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