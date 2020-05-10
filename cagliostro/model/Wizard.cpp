/*
This file is part of cagliostro.
Copyright (C) 2020 Christopher Gundler <christopher@gundler.de>

This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>.
*/

#include "Wizard.h"
#include "Entity.h"

namespace cagliostro::model {

Wizard::Wizard(QString participant, QFile *output, const QString &title, QObject *parent)
	: QObject(parent), participant_(std::move(participant)), output_(output) {
  assert(!participant_.isEmpty());

  this->setObjectName(title);

  // Ensure that output lives as long as the text streams
  output->setParent(this);
  output_ << "SUBJECT\tQUESTION (" << title << ")\tANSWER\n";
}

QVector<Page *> Wizard::pages() noexcept {
  return Entity::get<Page>(this);
}

QString Wizard::participant() const noexcept {
  return participant_;
}

bool Wizard::save(Question *question) {
  if (question == nullptr || !*question) {
	return false;
  }

  // Write answer to disk
  output_ << participant_ << '\t' << question->fullName() << '\t' << question->answer() << '\n';
  output_.flush();
  return output_.status() == QTextStream::Status::Ok;;
}

Wizard *Wizard::load(QString participant, const QString &output_path, const QString &title, QObject *parent) {
  if (participant.isEmpty()) {
	return nullptr;
  }

  // Try to open the file
  auto *file = new QFile(output_path);
  if (file->exists() || !file->open(QIODevice::WriteOnly | QIODevice::Text)) {
	file->deleteLater();
	return nullptr;
  }

  return new Wizard(std::move(participant), file, title, parent);
}

}