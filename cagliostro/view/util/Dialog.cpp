/*
This file is part of cagliostro.
Copyright (C) 2020 Christopher Gundler <christopher@gundler.de>

This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>.
*/

#include "Dialog.h"

#include <QPushButton>
#include <QKeyEvent>
#include <QCoreApplication>

namespace cagliostro::view::util {

Dialog::Dialog(QWidget *parent) :
    QWidget(parent),
    pages_(new QStackedWidget(this)),
    buttons_(nullptr),
    abort_(false),
    initialized_(false) {

  auto next_button = new QPushButton(tr("Next"));
  next_button->setDisabled(true);
  next_button->setDefault(true);
  next_button->setAutoDefault(true);
  QObject::connect(next_button, &QPushButton::clicked, this, &Dialog::nextPage);
  buttons_ = new ButtonGroup({next_button}, this);

  auto *layout = new QVBoxLayout();
  layout->addWidget(pages_);
  layout->addWidget(buttons_);
  this->setLayout(layout);
}

ButtonGroup *Dialog::buttons() noexcept {
  return buttons_;
}

int Dialog::registerPage(QWidget *widget) {
  if (widget == nullptr) {
    return -1;
  }

  widget->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
  widget->setVisible(false);
  return pages_->addWidget(widget);
}

bool Dialog::setPageReady(int id, bool ready) {
  const int current_index = pages_->currentIndex();
  if (id != current_index && id != current_index + 1) {
    return false;
  }

  auto button = buttons_->button(NEXT_BUTTON);
  button->setDisabled(!ready);
  return true;
}

void Dialog::nextPage() {
  if (initialized_) {
    emit this->leavePage(pages_->currentIndex());
  }

  // Allow the page after being notified to abort leaving.
  if (abort_) {
    abort_ = false;
    return;
  }

  const int current_index = initialized_ ? pages_->currentIndex() : -1;
  if (current_index < pages_->count() - 1) {
	auto *next_button = buttons_->button(NEXT_BUTTON);
	next_button->setEnabled(false);
	if (current_index + 2 == pages_->count()) {
	  next_button->setText(tr("Finish"));
	}

	emit this->enterPage(current_index + 1);
	pages_->setCurrentIndex(current_index + 1);

	// Scale to best size for current page
	auto current_widget = pages_->currentWidget();
	current_widget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	current_widget->adjustSize();
	current_widget->setVisible(true);
  } else {
    this->close();
  }
}

void Dialog::showEvent(QShowEvent *event) {
  QWidget::showEvent(event);

  // Show the first page, once the widget itself is shown
  if (!initialized_) {
    this->prepare();
  }
}

bool Dialog::abortLeave(int id) {
  if (pages_->currentIndex() != id) {
    return false;
  }
  abort_ = true;
  return true;
}

void Dialog::prepare() {
  this->nextPage();
  initialized_ = true;
}

void Dialog::keyPressEvent(QKeyEvent *event) {
  if (event->key() == Qt::Key_Escape) {
    QCoreApplication::quit();
  } else {
    QWidget::keyPressEvent(event);
  }
}

}