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
#include <QAbstractButton>

namespace cagliostro::view {

Wizard::Wizard(QWidget *parent) : QWizard(parent) {
  // Set the general config
  this->setWindowTitle(tr("Cagliostro"));
  this->setTitleFormat(Qt::MarkdownText);
  this->setSubTitleFormat(Qt::MarkdownText);
  this->setOption(QWizard::NoCancelButton, true);
  this->setOption(QWizard::HaveHelpButton, false);
  this->setOption(QWizard::HelpButtonOnRight, false);
  this->setOption(QWizard::HaveCustomButton1, true);

  auto *config = new ConfigPage(this);
  QObject::connect(config, &ConfigPage::modelLoaded, this, &Wizard::createView);
  this->addPage(config);
}

void Wizard::createView(model::Wizard *wizard) {
  this->setWindowTitle(wizard->objectName());

  if (!wizard->includeQuestions()) {
	this->setButtonText(QWizard::FinishButton, tr("Exit"));
	this->button(QWizard::FinishButton)->setEnabled(true);
	QMessageBox::information(this, wizard->objectName(), wizard->completeMessage());
	return;
  }

  // Add the pages of wizard model
  for (auto *page: wizard->pages()) {
	this->addPage(new Page(page, this));
  }
}

}