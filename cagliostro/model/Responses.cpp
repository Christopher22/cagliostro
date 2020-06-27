/*
This file is part of cagliostro.
Copyright (C) 2020 Christopher Gundler <christopher@gundler.de>

This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>.
*/

#include "Responses.h"

#include <QFile>
#include <QDebug>
#include <QRegularExpression>

namespace cagliostro::model {

Responses *Responses::load(const QString &output, QString subject, QObject *parent) {
  auto *file = new QFile(output);
  if (!file->open(QIODevice::ReadWrite | QIODevice::Text | QIODevice::Append) || !file->seek(0)) {
    file->deleteLater();
    return nullptr;
  }

  return new Responses(file, std::move(subject), parent);
}

Responses::Responses(QIODevice *file, QString subject, QObject *parent)
    : QObject(parent), output_(file), subject_(std::move(subject)), num_responses_(0) {
  assert(file != nullptr && file->isOpen());
  file->seek(0);
  file->setParent(this);
  this->parse();
}

bool Responses::contains(QStringView page_name) const noexcept {
  const auto children = this->findChildren<Response *>(QRegularExpression(QString("%1\\/.+").arg(page_name)),
                                                       Qt::FindDirectChildrenOnly);
  return !children.isEmpty();
}

bool Responses::contains(Question *question) const noexcept {
  return this->findChild<Response *>(question->fullName(), Qt::FindDirectChildrenOnly) != nullptr;
}

bool Responses::contains(Page *page) const noexcept {
  return this->contains(page->objectName());
}

void Responses::parse() {
  if (!output_.atEnd()) {
    QString current_line;
    bool first_line = true;
    while (output_.readLineInto(&current_line)) {
      if (first_line) {
        first_line = false;
        continue;
      }

      // Skip empty lines
      if (current_line.trimmed().size() == 0) {
        continue;
      }

      // Try to parse a given response
      if (Response::create(current_line, num_responses_++, this) == nullptr) {
        qWarning() << "Unable to parse saved response: " << current_line;
      }
    }
  }
}

bool Responses::record(Question *question) {
  if (!question->isAnswered() || this->contains(question)) {
    return false;
  }

  // Write the header, if not already written
  if (output_.device()->size() == 0) {
    const auto *parent = this->parent();
    output_ << QString("DATETIME\tSUBJECT\tQUESTION (%1)\tANSWER\n").arg(parent != nullptr ? parent->objectName() : "");
  }

  auto *response = new Response(question, num_responses_++, this);
  response->write(output_, subject_);
  output_.flush();
  return output_.status() == QTextStream::Status::Ok;
}

bool Responses::record(Page *page) {
  bool result = true;
  const auto questions = page->questions();
  for (auto *i = questions.begin(), *end = questions.end(); i < end && result; ++i) {
    result = this->record(*i);
  }
  return result;
}

QVector<Responses::Response *> Responses::responses() noexcept {
  return Response::get<Response>(this);
}

Responses::Response *Responses::Response::create(const QString &line, int index, Responses *parent) {
  const auto elements = line.split('\t', Qt::SkipEmptyParts);
  if (elements.size() != 4) {
    return nullptr;
  }

  QDateTime date = QDateTime::fromString(elements[0], Qt::ISODate);
  if (!date.isValid()) {
    return nullptr;
  }

  return new Response(index, date, elements[2], elements[3], parent);
}

Responses::Response::Response(int index, QDateTime creation, const QString &name, QString answer, Responses *parent)
    : Entity(name, index, parent), creation_(std::move(creation)), answer_(std::move(answer)) {
  assert(parent != nullptr);
  this->setObjectName(name);
}

Responses::Response::Response(Question *question, int index, Responses *parent)
    : Entity(question->fullName(), index, parent),
      creation_(QDateTime::currentDateTime()),
      answer_(question->answer()) {
  assert(parent != nullptr);
  assert(question != nullptr);
  assert(question->isAnswered());
}

void Responses::Response::write(QTextStream &stream, QStringView subject) const {
  stream << creation_.toString(Qt::ISODate) << '\t' << subject << '\t' << this->objectName() << '\t' << answer_ << '\n';
}

}