/*
This file is part of cagliostro.
Copyright (C) 2020 Christopher Gundler <christopher@gundler.de>

This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>.
*/

#ifndef CAGLIOSTRO_CAGLIOSTRO_VIEW_SCALE_H_
#define CAGLIOSTRO_CAGLIOSTRO_VIEW_SCALE_H_

#include <QWidget>

#include "../model/Selection.h"

namespace cagliostro::view {
class Scale : public QWidget {
 Q_OBJECT
  Q_PROPERTY(QString selection READ selection NOTIFY valueSelected);

 public:
  explicit Scale(model::Selection *selection, QWidget *parent = nullptr);
  [[nodiscard]] QString selection() const noexcept;

 signals:
  void valueSelected();

 private:
  model::Selection *selection_;
};
}

#endif //CAGLIOSTRO_CAGLIOSTRO_VIEW_SCALE_H_
