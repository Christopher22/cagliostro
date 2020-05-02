/*
This file is part of cagliostro.
Copyright (C) 2020 Christopher Gundler <christopher@gundler.de>

This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>.
*/

#include "Page.h"
#include "../model/Selection.h"

#include <QVBoxLayout>
#include <QFormLayout>
#include <QComboBox>

namespace cagliostro::view {

Page::Page(model::Page *page, QWidget *parent) : QWizardPage(parent) {
  this->setTitle(page->title());
  this->setSubTitle(page->description());
  this->setButtonText(QWizard::NextButton, page->nextText());

  auto *layout = new QVBoxLayout();
  // ToDo: Add content widget
  auto *question_layout = new QFormLayout();
  for (auto *question: page->questions()) {
    QWidget *widget = this->createQuestionWidget(question);
    if (widget != nullptr) {
      question_layout->addRow(question->text(), widget);
      // ToDo: Ensure the name is unique and probably even add an * after
      this->registerField(question->objectName(), widget);
    }
  }
  layout->addLayout(question_layout);
  this->setLayout(layout);
}

QWidget *Page::createQuestionWidget(model::Question *question) const noexcept {
  auto *selection = qobject_cast<model::Selection *>(question);
  if (selection == nullptr) {
    return nullptr;
  }

  auto *widget = new QComboBox((QWidget *) this);
  widget->setModel(selection->operator->());
  return widget;
}

}