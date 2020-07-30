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

#include <opencv2/imgproc.hpp>

namespace cagliostro::model::content::util {

VideoDecoder::VideoDecoder(std::unique_ptr<cv::VideoCapture> player)
	: QObject(),
	  raw_decoder_(std::move(player)),
	  buffer_(),
	  timer_(new QTimer(this)),
	  surface_(nullptr),
	  milliseconds_per_frame_(16) {

  const double fps = raw_decoder_->get(cv::CAP_PROP_FPS);
  if (fps > 0) {
	milliseconds_per_frame_ = 1000 / fps;
  }

  timer_->setInterval(milliseconds_per_frame_);
  QObject::connect(timer_, &QTimer::timeout, this, &VideoDecoder::decode);
  QObject::connect(this, &VideoDecoder::done, this, &VideoDecoder::close);
  QObject::connect(this, &VideoDecoder::started, timer_, [&]() {
	timer_->start();
  });
  QObject::connect(this, &VideoDecoder::stopped, timer_, [&]() {
	if (timer_->isActive()) {
	  timer_->stop();
	}
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
  if (!raw_decoder_->read(raw_buffer_)) {
	emit this->done();
	return false;
  }

  const std::size_t bytes = raw_buffer_.cols * raw_buffer_.rows * 4;
  qint64 start, stop;
  if (buffer_.isValid()) {
	emit this->frameReady(buffer_);
	start = buffer_.startTime() + milliseconds_per_frame_;
	stop = buffer_.endTime() + milliseconds_per_frame_;
  } else {
	start = 0.0;
	stop = milliseconds_per_frame_;
  }

  // At this place, we allocate a new video frame. Oterwise, we may overwrite the content during presenation.
  buffer_ = QVideoFrame(bytes,
						QSize(raw_buffer_.cols, raw_buffer_.rows),
						raw_buffer_.cols * 4,
						QVideoFrame::Format_ARGB32);
  buffer_.setStartTime(start);
  buffer_.setEndTime(stop);

  if (!buffer_.isValid() || !buffer_.map(QAbstractVideoBuffer::ReadWrite)) {
	buffer_ = QVideoFrame();
	return false;
  }

  cv::cvtColor(raw_buffer_, converted_buffer_, cv::COLOR_BGR2BGRA);
  std::memcpy(buffer_.bits(), converted_buffer_.data, bytes);

  buffer_.unmap();

  return true;
}

bool VideoDecoder::start() {
  assert(QThread::currentThread() != this->thread());

  if (this->status() != Status::Stopped || !this->decode()) {
	return false;
  }

  emit this->started();
  return true;
}

bool VideoDecoder::stop(bool should_close) {
  assert(QThread::currentThread() != this->thread());
  if (this->status() != Status::Running) {
	return false;
  }

  if (should_close) {
	emit this->done();
  } else {
	emit this->stopped();
  }
  return true;
}

void VideoDecoder::setVideoSurface(QAbstractVideoSurface *surface) {
  assert(QThread::currentThread() != this->thread());

  if (surface_ != surface && surface_ && surface_->isActive()) {
	surface_->stop();
  }

  surface_ = surface;
  if (surface_) {
	QObject::connect(this, &VideoDecoder::started, surface_, [this]() {
	  if (!surface_->start(QVideoSurfaceFormat(
		  buffer_.size(),
		  buffer_.pixelFormat(),
		  QAbstractVideoBuffer::NoHandle
	  ))) {
		qWarning("Unable to start the video surface!");
	  }
	});

	QObject::connect(this, &VideoDecoder::frameReady, surface_, [this](const QVideoFrame &frame) {
	  if (surface_->isActive() && !surface_->present(frame)) {
		qWarning("Unable to present frame");
	  }
	});
  }
}

QSize VideoDecoder::size() const noexcept {
  return buffer_.size();
}

void VideoDecoder::close() {
  if (timer_->isActive()) {
	timer_->stop();
  }
  raw_decoder_.reset(nullptr);
}

VideoDecoder::Status VideoDecoder::status() const noexcept {
  if (timer_->isActive()) {
	return Status::Running;
  } else if (raw_decoder_) {
	return Status::Stopped;
  } else {
	return Status::Closed;
  }
}

}