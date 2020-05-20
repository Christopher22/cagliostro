/*
This file is part of cagliostro.
Copyright (C) 2020 Christopher Gundler <christopher@gundler.de>

This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>.
*/

#ifndef CAGLIOSTRO_CAGLIOSTRO_MODEL_RESPONSES_H_
#define CAGLIOSTRO_CAGLIOSTRO_MODEL_RESPONSES_H_

#include "Page.h"

#include <QIODevice>
#include <QTextStream>
#include <QDateTime>

namespace cagliostro::model {
class Responses : public QObject {
 Q_OBJECT

 public:
  class Response : public Entity {
   public:
	friend class Responses;
	void write(QTextStream &stream, QStringView subject) const;

   protected:
	static Response *create(const QString &line, int index, Responses *parent);
	Response(Question *question, int index, Responses *parent);

   private:
	Response(int index, QDateTime creation, const QString &name, QString answer, Responses *parent);

	QDateTime creation_;
	QString answer_;
  };

  Responses(QIODevice *file, QString subject, QObject *parent);
  static Responses *load(const QString &file, QString subject, QObject *parent);
  [[nodiscard]] QVector<Response *> responses() noexcept;
  [[nodiscard]] bool contains(Page *page) const noexcept;
  [[nodiscard]] bool contains(QStringView page_name) const noexcept;
  [[nodiscard]] bool contains(Question *question) const noexcept;

  bool record(Question *question);
  bool record(Page *page);

 private:
  QTextStream output_;
  QString subject_;
  int num_responses_;

  void parse();
};
}

#endif //CAGLIOSTRO_CAGLIOSTRO_MODEL_RESPONSES_H_
