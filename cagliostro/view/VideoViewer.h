/*
This file is part of cagliostro.
Copyright (C) 2020 Christopher Gundler <christopher@gundler.de>

This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>.
*/

#ifndef CAGLIOSTRO_CAGLIOSTRO_VIEW_VIDEOVIEWER_H_
#define CAGLIOSTRO_CAGLIOSTRO_VIEW_VIDEOVIEWER_H_

#include <QLabel>
#include <QAbstractVideoSurface>
#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>
#include <QImage>

namespace cagliostro::view {

class VideoViewer : public QOpenGLWidget, protected QOpenGLFunctions {
 Q_OBJECT

 public:
  explicit VideoViewer(const QSize &size, QWidget *parent = nullptr);
  QAbstractVideoSurface *surface();
  inline QOpenGLTexture *frame() noexcept {
    return texture_;
  }

 protected:
  void initializeGL() override;
  void paintGL() override;

 private:
  class VideoSurface : public QAbstractVideoSurface {
   public:
    explicit VideoSurface(VideoViewer *parent);

    bool present(const QVideoFrame &frame) override;
    bool start(const QVideoSurfaceFormat &format) override;
    [[nodiscard]] QList<QVideoFrame::PixelFormat> supportedPixelFormats(QAbstractVideoBuffer::HandleType handleType) const override;
    [[nodiscard]] bool isFormatSupported(const QVideoSurfaceFormat &format) const override;
  };

  bool initTextures();
  bool initShaders();

  VideoSurface *surface_;
  QVector<QVector3D> vertex_coordinates_;
  QVector<QVector2D> texture_coordinates_;
  QOpenGLShaderProgram program;
  QOpenGLTexture *texture_;
  QSize size_;

  constexpr static const char *VERTEX_SHADER = R"|(
attribute vec4 vertex;
attribute vec2 texCoord;
varying vec2 texc;
void main(void) {
    gl_Position = vertex;
    texc = texCoord;
})|";

  constexpr static const char *FRAGMENT_SHADER = R"|(
uniform sampler2D texture;
varying vec2 texc;
void main(void) {
  gl_FragColor = texture2D(texture,texc);
})|";
};
}

#endif //CAGLIOSTRO_CAGLIOSTRO_MODEL_CONTENT_UTIL_VIDEOVIEWER_H_
