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
	: QOpenGLWidget(parent),
	  surface_(new VideoSurface(this)),
	  size_(size),
	  texture_(nullptr),
	  texture_coordinates_({
							   QVector2D(0, 1),
							   QVector2D(1, 1),
							   QVector2D(0, 0),
							   QVector2D(1, 0)}),
	  vertex_coordinates_({
							  QVector3D(-1, -1, 1),
							  QVector3D(1, -1, 1),
							  QVector3D(-1, 1, 1),
							  QVector3D(1, 1, 1)}
	  ) {

  this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}

bool VideoViewer::initTextures() {
  texture_ = new QOpenGLTexture(QOpenGLTexture::Target2D);
  texture_->setMinMagFilters(QOpenGLTexture::Nearest, QOpenGLTexture::Linear);
  texture_->setWrapMode(QOpenGLTexture::Repeat); // Reuse texture coordinates (1.1, 1.2) = (0.1, 0.2)
  texture_->setSize(size_.width(), size_.height(), 3);
  texture_->setFormat(QOpenGLTexture::RGBFormat);
  texture_->allocateStorage(QOpenGLTexture::BGR, QOpenGLTexture::UInt8);
  return texture_->isStorageAllocated();
}

bool VideoViewer::initShaders() {
  auto *vshader = new QOpenGLShader(QOpenGLShader::Vertex, this);
  //Compile vertex shader code
  if (!vshader->compileSourceCode(VERTEX_SHADER)) {
	return false;
  }

  auto *fshader = new QOpenGLShader(QOpenGLShader::Fragment, this);
  //Compile texture shader code
  if (!fshader->compileSourceCode(FRAGMENT_SHADER)) {
	return false;
  }

  program.addShader(vshader);//Add vertex shader
  program.addShader(fshader);//Add texture fragment shader
  program.bindAttributeLocation("vertex_coordinates", 0);//bind vertex attribute location
  program.bindAttributeLocation("texture_coordinates", 1);//Bound texture attribute location

  // Link and bind shader pipeline
  return program.link() && program.bind();
}

void VideoViewer::initializeGL() {
  this->initializeOpenGLFunctions(); //Initialize OPenGL function
  this->glClearColor(0, 0, 0, 0); //Set the background to black
  this->glEnable(GL_CULL_FACE);
  this->glEnable(GL_TEXTURE_2D);
  if (!this->initTextures()) {
	qWarning("Unable to initialize textures.");
  }
  if (!this->initShaders()) {
	qWarning("Unable to initialize shaders.");
  }
}

void VideoViewer::paintGL() {
  //Clear screen buffer and depth buffer
  this->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  //Bound texture
  texture_->bind();

  program.enableAttributeArray(0);
  program.enableAttributeArray(1);
  program.setAttributeArray(0, vertex_coordinates_.constData());
  program.setAttributeArray(1, texture_coordinates_.constData());
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

QAbstractVideoSurface *VideoViewer::surface() {
  return surface_;
}

void VideoViewer::showEvent(QShowEvent *event) {
  QWidget::showEvent(event);

  // Enforce OpenGL to render
  this->update();
}

QSize VideoViewer::sizeHint() const {
  return size_;
}

VideoViewer::VideoSurface::VideoSurface(VideoViewer *parent) : QAbstractVideoSurface(parent) {}

bool VideoViewer::VideoSurface::present(const QVideoFrame &frame) {
  auto viewer = qobject_cast<VideoViewer *>(this->parent());
  auto texture = viewer->frame();
  if (texture == nullptr || !frame.isMapped() || !frame.isValid()) {
	return false;
  }

  texture->setData(QOpenGLTexture::BGR, QOpenGLTexture::UInt8, frame.bits());
  viewer->update();
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