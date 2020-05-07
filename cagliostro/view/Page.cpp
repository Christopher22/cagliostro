/*
This file is part of cagliostro.
Copyright (C) 2020 Christopher Gundler <christopher@gundler.de>

This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>.
*/

#include "Page.h"
#include "VideoViewer.h"
#include "../model/Selection.h"
#include "../model/content/Video.h"

#include <QVBoxLayout>
#include <QFormLayout>
#include <QComboBox>
#include <QVideoWidget>

namespace cagliostro::view {

Page::Page(model::Page *page, QWidget *parent) : QWizardPage(parent), page_(page) {
  this->setTitle(page->title());
  this->setSubTitle(page->description());
  this->setButtonText(QWizard::NextButton, page->nextText());

  // Create the layout and add the content widget, if provided
  auto *layout = new QVBoxLayout();
  QWidget *content = this->createContentWidget(page->content());
  if (content != nullptr) {
	layout->addWidget(content);
  }

  // Create the question layout
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

QWidget *Page::createQuestionWidget(model::Question *question) noexcept {
  auto *selection = qobject_cast<model::Selection *>(question);
  if (selection == nullptr) {
	return nullptr;
  }

  auto *widget = new QComboBox(this);
  widget->setModel(selection->operator->());
  return widget;
}

QWidget *Page::createContentWidget(model::content::Content *content) noexcept {
  auto *video = qobject_cast<model::content::Video *>(content);
  if (video == nullptr) {
	return nullptr;
  }

  auto *widget = new VideoViewer(video->size(), this);
  if (!video->bind(widget->surface())) {
	widget->deleteLater();
	return nullptr;
  }
  return widget;
}

void Page::initializePage() {
  // WARNING: This is called only once, not on backward.
  QWizardPage::initializePage();

  // Play the video on start
  auto *content = this->page_->content();
  if (content != nullptr) {
	content->show();
  }
}

bool Page::validatePage() {
  const auto result = QWizardPage::validatePage();

  auto *content = this->page_->content();
  if (result && content != nullptr) {
	content->hide();
  }
  return result;
}

}