/*
This file is part of cagliostro.
Copyright (C) 2020 Christopher Gundler <christopher@gundler.de>

This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>.
*/

#include "Page.h"
#include "VideoViewer.h"
#include "Scale.h"
#include "../model/content/Video.h"

#include <QVBoxLayout>
#include <QFormLayout>
#include <QComboBox>
#include <QAbstractButton>
#include <QMessageBox>

namespace cagliostro::view {

Page::Page(model::Page *page, QWizard *parent) : QWizardPage(parent), page_(page) {
  this->setTitle(page->title());
  this->setSubTitle(page->description());
  this->setCommitPage(true);

  // Create the layout and add the content widget, if provided
  auto *layout = new QVBoxLayout();
  layout->setAlignment(Qt::AlignCenter);

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

  auto *widget = new Scale(selection, this);
  this->registerField(question->fullName(true), widget, "selection", SIGNAL(valueSelected()));
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

  // Inform the widget once the content was completely played
  QObject::connect(content, &model::content::Content::finished, this, &Page::completeChanged);
  return widget;
}

void Page::initializePage() {
  // WARNING: This is called only once, not on backward.
  QWizardPage::initializePage();

  // Set the corresponding button text
  this->setButtonText(QWizard::CommitButton, page_->nextText());

  // Play the video on start
  auto *content = this->page_->content();
  if (content != nullptr) {
    content->show();
  }
}

bool Page::validatePage() {
  if (!QWizardPage::validatePage()) {
    return false;
  }

  // Stop playing the video
  auto *content = page_->content();
  if (content != nullptr) {
    content->hide();
  }

  // Save questions
  if (!page_->save()) {
    QMessageBox::critical(this,
                          "Saving failed",
                          "Logging your recent answer(s) failed. Please contact your supervisor.");
  }

  return true;
}

bool Page::isComplete() const {
  const auto *content = page_->content();
  return QWizardPage::isComplete() && (content == nullptr || !content->isObligatory() || content->isFinished());
}

}