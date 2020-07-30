/*
This file is part of cagliostro.
Copyright (C) 2020 Christopher Gundler <christopher@gundler.de>

This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>.
*/

#include "DialogPage.h"

namespace cagliostro::view::util {

DialogPage::DialogPage(Dialog *dialog) : QWidget(nullptr), dialog_(dialog) {
  assert(dialog != nullptr);
  const int id = dialog->registerPage(this);
  assert(id >= 0);

  page_id_ = id;
  QObject::connect(this, &DialogPage::ready, this, [this](bool value) {
    dialog_->setPageReady(page_id_, value);
  });
  QObject::connect(dialog_, &Dialog::enterPage, this, &DialogPage::handleEnterSignal);
  QObject::connect(dialog_, &Dialog::enteredPage, this, &DialogPage::handleEnteredSignal);
  QObject::connect(dialog_, &Dialog::leavePage, this, &DialogPage::handleLeaveSignal);
}

void DialogPage::handleEnterSignal(int id) {
  if (page_id_ == id) {
    this->prepare();
  }
}

void DialogPage::handleLeaveSignal(int id) {
  // Abort leaving the page if cleanUp failed
  if (page_id_ == id && !this->cleanUp()) {
	dialog_->abortLeave(page_id_);
  }
}

void DialogPage::handleEnteredSignal(int id) {
  if (page_id_ == id) {
	this->present();
  }
}

ButtonGroup *DialogPage::buttons() noexcept {
  return dialog_->buttons();
}

}