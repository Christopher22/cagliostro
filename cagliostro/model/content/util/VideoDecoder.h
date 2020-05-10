/*
This file is part of cagliostro.
Copyright (C) 2020 Christopher Gundler <christopher@gundler.de>

This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>.
*/

#ifndef CAGLIOSTRO_CAGLIOSTRO_MODEL_CONTENT_UTIL_VIDEODECODER_H_
#define CAGLIOSTRO_CAGLIOSTRO_MODEL_CONTENT_UTIL_VIDEODECODER_H_

#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>

#include <QUrl>
#include <QObject>
#include <QImage>
#include <QTimer>
#include <QAbstractVideoSurface>

namespace cagliostro::model::content::util {
class VideoDecoder : public QObject {
 Q_OBJECT
  Q_PROPERTY(QAbstractVideoSurface *videoSurface READ videoSurface WRITE setVideoSurface)
  Q_PROPERTY(QSize size READ size)

 public:
  static VideoDecoder *load(const QUrl &uri);
  bool start();
  bool stop();

  [[nodiscard]] inline QAbstractVideoSurface *videoSurface() const { return surface_; }
  void setVideoSurface(QAbstractVideoSurface *surface);
  [[nodiscard]] QSize size() const noexcept;

 signals:
  void started();
  void frameReady(const QVideoFrame &frame);
  void done();
  void stopped();

 protected:
  explicit VideoDecoder(std::unique_ptr<cv::VideoCapture> player);
  bool decode();

 private:
  std::unique_ptr<cv::VideoCapture> raw_decoder_;
  QVideoFrame buffer_;
  QTimer *timer_;
  QAbstractVideoSurface *surface_;
  qint64 milliseconds_per_frame_;
};
}

#endif //CAGLIOSTRO_CAGLIOSTRO_MODEL_CONTENT_UTIL_VIDEODECODER_H_
