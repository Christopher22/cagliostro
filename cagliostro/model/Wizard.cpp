/*
This file is part of cagliostro.
Copyright (C) 2020 Christopher Gundler <christopher@gundler.de>

This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>.
*/

#include "Wizard.h"

namespace cagliostro::model {

Wizard::Wizard(QString participant, const QString &title, QString complete_message, QObject *parent)
	: QObject(parent), participant_(std::move(participant)), complete_message_(std::move(complete_message)) {
  assert(!participant_.isEmpty());

  this->setObjectName(title);
}

QVector<Page *> Wizard::pages() const noexcept {
  return Entity::get<Page>(this);
}

QString Wizard::participant() const noexcept {
  return participant_;
}

QString Wizard::completeMessage() const noexcept {
  return complete_message_;
}

bool Wizard::save(Page *page) {
  if (page == nullptr || page->parent() != this) {
	qWarning("The registered page is not valid");
	return false;
  }

  auto *responses = this->responses();
  if (responses == nullptr || responses->contains(page)) {
	return false;
  }

  return responses->record(page);
}

Responses *Wizard::responses() noexcept {
  return this->findChild<Responses *>(QString(), Qt::FindDirectChildrenOnly);
}

bool Wizard::includeQuestions() const noexcept {
  // Figure out if there are still questions unanswered
  for (auto *page: this->pages()) {
	if (!page->questions().empty()) {
	  return true;
	}
  }
  return false;
}
}