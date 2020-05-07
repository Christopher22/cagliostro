/*
This file is part of cagliostro.
Copyright (C) 2020 Christopher Gundler <christopher@gundler.de>

This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>.
*/

#include "VideoDecoder.h"

#include <QVideoSurfaceFormat>
#include <QThread>

namespace cagliostro::model::content::util {

VideoDecoder::VideoDecoder(std::unique_ptr<cv::VideoCapture> player)
	: QObject(),
	  raw_decoder_(std::move(player)),
	  buffer_(),
	  timer_(new QTimer(this)),
	  surface_(nullptr),
	  milliseconds_per_frame(16) {

  const double fps = raw_decoder_->get(cv::CAP_PROP_FPS);
  if (fps > 0) {
	milliseconds_per_frame = 1000 / fps;
  }

  timer_->setInterval(milliseconds_per_frame);
  QObject::connect(timer_, &QTimer::timeout, this, &VideoDecoder::decode);
  QObject::connect(this, &VideoDecoder::done, timer_, &QTimer::stop);
  QObject::connect(this, &VideoDecoder::started, timer_, [&]() {
	timer_->start();
  });
  QObject::connect(this, &VideoDecoder::stopped, timer_, [&]() {
	timer_->stop();
  });
}

VideoDecoder *VideoDecoder::load(const QUrl &uri) {
  // Open the video
  auto path = uri.isLocalFile() ? uri.toLocalFile() : uri.toString();
  auto raw_decoder = std::make_unique<cv::VideoCapture>(path.toStdString(), cv::CAP_FFMPEG);
  if (!raw_decoder->isOpened()) {
	return nullptr;
  }

  // Decode the first frame to ensure the video is readable
  auto *decoder = new VideoDecoder(std::move(raw_decoder));
  if (!decoder->decode()) {
	delete decoder;
	return nullptr;
  }

  return decoder;
}

bool VideoDecoder::decode() {
  const bool must_create_buffer = !buffer_.isValid();
  cv::Mat buffer;
  if (!must_create_buffer) {
	emit this->frameReady(buffer_);
	// Prepare the next frame
	buffer_.setStartTime(buffer_.startTime() + milliseconds_per_frame);
	buffer_.setEndTime(buffer_.endTime() + milliseconds_per_frame);
	buffer = cv::Mat(buffer_.height(), buffer_.width(), CV_8UC3, buffer_.bits(), buffer_.bytesPerLine());
  }

  if (!raw_decoder_->read(buffer)) {
	emit this->done();
	return false;
  }

  if (must_create_buffer) {
	const std::size_t bytes = buffer.step[0] * buffer.rows;
	buffer_ = QVideoFrame(bytes, QSize(buffer.cols, buffer.rows), buffer.step, QVideoFrame::Format_BGR24);
	if (!buffer_.map(QAbstractVideoBuffer::ReadWrite)) {
	  buffer_ = QVideoFrame();
	  return false;
	}
	std::memcpy(buffer_.bits(), buffer.data, bytes);

	// Set buffer metadata
	buffer_.setStartTime(0);
	buffer_.setEndTime(milliseconds_per_frame);
  }

  return true;
}

bool VideoDecoder::start() {
  assert(QThread::currentThread() != this->thread());

  if (timer_->isActive() || !this->decode()) {
	return false;
  }

  emit this->started();
  return true;
}

bool VideoDecoder::stop() {
  assert(QThread::currentThread() != this->thread());

  if (!timer_->isActive()) {
	return false;
  }

  emit this->stopped();
  return true;
}

void VideoDecoder::setVideoSurface(QAbstractVideoSurface *surface) {
  assert(QThread::currentThread() != this->thread());

  if (surface_ != surface && surface_ && surface_->isActive()) {
	surface_->stop();
  }

  surface_ = surface;
  if (surface_) {
	surface_->start(QVideoSurfaceFormat(
		buffer_.size(),
		buffer_.pixelFormat(),
		QAbstractVideoBuffer::NoHandle
	));
	QObject::connect(this, &VideoDecoder::frameReady, surface_, [this](const QVideoFrame &frame) {
	  surface_->present(frame);
	});
  }
}

QSize VideoDecoder::size() const noexcept {
  return buffer_.size();
}

}