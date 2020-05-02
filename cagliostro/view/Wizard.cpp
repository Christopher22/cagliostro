/*
This file is part of cagliostro.
Copyright (C) 2020 Christopher Gundler <christopher@gundler.de>

This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>.
*/

#include "Wizard.h"
#include "Page.h"

namespace cagliostro::view {

Wizard::Wizard(model::Wizard *wizard, QWidget *parent) : QWizard(parent) {
  // Set the general config
  this->setWindowTitle(wizard->objectName());
  this->setTitleFormat(Qt::MarkdownText);
  this->setSubTitleFormat(Qt::MarkdownText);

  // Add the pages of wizard model
  for (auto *page: wizard->pages()) {
    this->addPage(new Page(page, this));
  }
}

}