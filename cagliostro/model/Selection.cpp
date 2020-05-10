/*
This file is part of cagliostro.
Copyright (C) 2020 Christopher Gundler <christopher@gundler.de>

This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>.
*/

#include "Selection.h"

namespace cagliostro::model {

Selection::Selection(QString text, const QStringList &choices, const QString &name, int index, QObject *parent)
	: Question(std::move(text),
			   name,
			   index,
			   parent),
	  choices_(new QStringListModel(choices, this)),
	  answer_(QPersistentModelIndex(QModelIndex())) {

  if (choices.contains("")) {
	qWarning("An empty choice is not supported");
  }
}

QStringListModel *Selection::operator->() noexcept {
  return choices_;
}

bool Selection::isAnswered() const {
  return answer_.isValid();
}

bool Selection::setAnswer(int answer_index) {
  auto index = choices_->index(answer_index);
  if (!index.isValid() || index == answer_) {
	return false;
  }

  answer_ = QPersistentModelIndex(index);
  emit this->answered();
  return true;
}

bool Selection::setAnswer(const QString &answer) {
  // An answer can not be empty - it would be an ambiguous invalid answer.
  if (answer.isEmpty()) {
	return false;
  }

  const auto candidates = choices_->match(choices_->index(0, 0), Qt::DisplayRole, answer);
  if (candidates.size() == 1) {
	return this->setAnswer(candidates[0].row());
  } else {
	return false;
  }
}

QString Selection::answer() const noexcept {
  return this->isAnswered() ? answer_.data().toString() : "";
}

}