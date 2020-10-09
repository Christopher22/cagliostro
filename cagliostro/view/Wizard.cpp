/*
This file is part of cagliostro.
Copyright (C) 2020 Christopher Gundler <christopher@gundler.de>

This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>.
*/

#include "Wizard.h"
#include "Page.h"
#include "ConfigPage.h"

#include <QMessageBox>
#include <QApplication>
#include <QScreen>

namespace cagliostro::view {

Wizard::Wizard(QWidget *parent) : util::Dialog(parent), wizard_(nullptr) {
  // Set the general config
  this->setWindowTitle(tr("Cagliostro"));
  // The experiment should be shown before any other window and shall only show a close button
  this->setWindowFlags(Qt::CustomizeWindowHint | Qt::WindowStaysOnTopHint | Qt::WindowCloseButtonHint);
  // Limit window size to maximal size of the primary screen
  this->setMaximumSize(QApplication::screenAt(this->geometry().center())->geometry().size());

  auto *config = new ConfigPage(this);
  QObject::connect(config, &ConfigPage::modelLoaded, this, &Wizard::createView);
  QObject::connect(this, &Wizard::enterPage, this, [this](int id) {
	if (id == 1) {
	  auto font = QApplication::font();
	  font.setPointSize(wizard_->fontSize());
	  QApplication::setFont(font);

	  this->setGeometry(QApplication::screenAt(this->geometry().center())->availableGeometry());
	  this->showMaximized();
	}
  });
}

void Wizard::createView(model::Wizard *wizard) {
  wizard_ = wizard;

  this->setWindowTitle(wizard->objectName());

  if (!wizard->includeQuestions()) {
    auto button = this->buttons()->button(NEXT_BUTTON);
    button->setText(tr("Exit"));
    button->setEnabled(true);
    QMessageBox::information(this, wizard->objectName(), wizard->completeMessage());
    return;
  }

  // Add the pages of wizard model
  for (model::Page *page: wizard->pages()) {
    new Page(page, this);
  }
}
}