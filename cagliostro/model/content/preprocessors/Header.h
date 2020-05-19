/*
This file is part of cagliostro.
Copyright (C) 2020 Christopher Gundler <christopher@gundler.de>

This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>.
*/

#ifndef CAGLIOSTRO_CAGLIOSTRO_MODEL_CONTENT_PREPROCESSORS_HEADER_H_
#define CAGLIOSTRO_CAGLIOSTRO_MODEL_CONTENT_PREPROCESSORS_HEADER_H_

#include "../Resource.h"

namespace cagliostro::model::content {
class Header : public QObject {
 Q_OBJECT

 public:
  static Header *create(const QString &header_info, Resource *resource);

 protected:
  Header(QByteArray before, QByteArray after, Resource *resource);

 private:
  QByteArray before_, after_;

  void changeHeader(Resource *resource, bool is_preparing);
};
}

#endif //CAGLIOSTRO_CAGLIOSTRO_MODEL_CONTENT_PREPROCESSORS_HEADER_H_
