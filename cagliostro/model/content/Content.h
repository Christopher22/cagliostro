/*
This file is part of cagliostro.
Copyright (C) 2020 Christopher Gundler <christopher@gundler.de>

This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>.
*/

#ifndef CAGLIOSTRO_CAGLIOSTRO_MODEL_CONTENT_CONTENT_H_
#define CAGLIOSTRO_CAGLIOSTRO_MODEL_CONTENT_CONTENT_H_

#include <QObject>
#include <QUrl>

#include "Resource.h"

namespace cagliostro::model::content {
class Content : public QObject {
 Q_OBJECT
  Q_PROPERTY(bool isObligatory READ isObligatory WRITE setObligatory NOTIFY obligatoryChanged);
  Q_PROPERTY(bool isFinished READ isFinished WRITE setFinished NOTIFY finished);

 public:
  virtual bool show() = 0;
  virtual void hide();

  [[nodiscard]] bool isObligatory() const noexcept;
  [[nodiscard]] bool isFinished() const noexcept;
  void setObligatory(bool is_obligatory);

 signals:
  void finished();
  void obligatoryChanged();

 protected:
  explicit Content(Resource *resource, QObject *parent = nullptr);
  [[nodiscard]] Resource *resource() const;
  void setFinished(bool finished = true);

 private:
  bool obligatory_, finished_;
};
}

#endif //CAGLIOSTRO_CAGLIOSTRO_MODEL_CONTENT_CONTENT_H_
