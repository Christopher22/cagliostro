/*
This file is part of cagliostro.
Copyright (C) 2020 Christopher Gundler <christopher@gundler.de>

This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>.
*/

#include "AspectRatioLayout.h"

#include <QWidget>

namespace cagliostro::view::util {

AspectRatioLayout::AspectRatioLayout(double aspect_ratio, QWidget *parent)
	: QLayout(parent), aspect_ratio_(aspect_ratio), content_(nullptr) {
}

AspectRatioLayout::~AspectRatioLayout() {
  delete content_;
}

void AspectRatioLayout::addItem(QLayoutItem *item) {
  delete content_;
  content_ = item;
  item->setAlignment(0);
}

QLayoutItem *AspectRatioLayout::takeAt(int) {
  QLayoutItem *retval = content_;
  content_ = nullptr;
  return retval;
}

void AspectRatioLayout::setGeometry(const QRect &rect) {
  QLayout::setGeometry(rect);
  if (content_ == nullptr) {
	return;
  }

  const auto margin = this->contentsMargins();
  const auto available_size = rect - 2 * margin;
  QWidget *widget = content_->widget();

  int height = available_size.height(), width = static_cast<int>(height * aspect_ratio_);
  if (width > available_size.width()) {
	// fill width
	width = available_size.width();
	height = static_cast<int>(width / aspect_ratio_);
	int y;
	if (content_->alignment() & Qt::AlignTop) {
	  y = margin.top();
	} else if (content_->alignment() & Qt::AlignBottom) {
	  y = rect.height() - margin.bottom() - height;
	} else {
	  y = margin.top() + (available_size.height() - height) / 2;
	}
	widget->setGeometry(rect.x() + margin.left(), rect.y() + y, width, height);
  } else {
	int x;
	if (content_->alignment() & Qt::AlignLeft) {
	  x = margin.left();
	} else if (content_->alignment() & Qt::AlignRight) {
	  x = rect.width() - margin.right() - width;
	} else {
	  x = margin.left() + (available_size.width() - width) / 2;
	}
	widget->setGeometry(rect.x() + x, rect.y() + margin.top(), width, height);
  }
}

QSize AspectRatioLayout::sizeHint() const {
  const int margins = 2 * this->margin();
  return content_ ? content_->sizeHint() + QSize(margins, margins) : QSize(margins, margins);
}

QSize AspectRatioLayout::minimumSize() const {
  const int margins = 2 * this->margin();
  return content_ ? content_->minimumSize() + QSize(margins, margins) : QSize(margins, margins);
}

int AspectRatioLayout::heightForWidth(int width) const {
  return static_cast<int>(static_cast<float>(width - 2 * this->margin()) / aspect_ratio_ + 2 * this->margin());
}

int AspectRatioLayout::count() const {
  return content_ != nullptr ? 1 : 0;
}

QLayoutItem *AspectRatioLayout::itemAt(int i) const {
  return i == 0 ? content_ : nullptr;
}

Qt::Orientations AspectRatioLayout::expandingDirections() const {
  return Qt::Horizontal | Qt::Vertical;
}

bool AspectRatioLayout::hasHeightForWidth() const {
  return false;
}

}