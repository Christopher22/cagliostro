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

#include <QComboBox>
#include <QMessageBox>
#include <QGuiApplication>
#include <QScreen>
#include <QLabel>

namespace cagliostro::view {

Page::Page(model::Page *page, util::Dialog *parent)
    : util::DialogPage(parent), page_(page), question_layout_(new QFormLayout()) {

  // Create the layout and add the content widget, if provided
  auto *layout = new QVBoxLayout();
  layout->setAlignment(Qt::AlignCenter);

  auto title = new QLabel(page->title(), this);
  title->setTextFormat(Qt::MarkdownText);
  layout->addWidget(title, 0, Qt::AlignLeft);

  auto description = new QLabel(page->description(), this);
  description->setTextFormat(Qt::MarkdownText);
  layout->addWidget(description, 0, Qt::AlignLeft);

  QWidget *renderer = this->createContentWidget(page->content());
  if (renderer != nullptr) {
    layout->addWidget(renderer);
  }

  // Create the question layout
  for (auto *question: page->questions()) {
    QWidget *widget = this->createQuestionWidget(question);
    if (widget != nullptr) {
      question_layout_->addRow(question->text(), widget);
    }
  }
  layout->addLayout(question_layout_);

  this->setLayout(layout);
}

QWidget *Page::createQuestionWidget(model::Question *question) noexcept {
  auto *selection = qobject_cast<model::Selection *>(question);
  if (selection == nullptr) {
    return nullptr;
  }

  auto *widget = new Scale(selection, this);
  QObject::connect(widget, &Scale::valueSelected, this, &Page::checkReady);
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
  QObject::connect(content, &model::content::Content::finished, this, &Page::checkReady);
  return widget;
}

void Page::prepare() {
  // Set the corresponding button text
  this->buttons()->button(0)->setText(page_->nextText());

  // Play the video on start
  auto *content = this->page_->content();
  if (content != nullptr) {
    content->show();
  }

  // If there are no questions, continue directly
  if (question_layout_->count() == 0) {
    emit this->ready(true);
  }
}

bool Page::cleanUp() {
  // Stop playing the video
  auto *content = page_->content();
  if (content != nullptr) {
    content->hide();
  }

  // Save questions
  if (!page_->save()) {
    QMessageBox::critical(this,
                          tr("Saving failed"),
                          tr("Logging your recent answer(s) failed. Please contact your supervisor."
                          ));
    return false;
  }

  return true;
}

void Page::checkReady() {
  // Check that the user have seen the content if it is obligatory
  const auto *content = page_->content();
  if (content == nullptr || !content->isObligatory() || content->isFinished()) {
    // Check if all questions are answered
    for (int i = 0, size = question_layout_->count(); i < size; ++i) {
      auto item = question_layout_->itemAt(i, QFormLayout::FieldRole);
      auto question = item != nullptr ? qobject_cast<Scale *>(item->widget()) : nullptr;
      if (question != nullptr && !*question) {
        // This question was not answered.
        return;
      }
    }

    // Inform the dialog that the user may continue
    emit this->ready(true);
  }
}

}