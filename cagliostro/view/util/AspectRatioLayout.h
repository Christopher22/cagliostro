/*
This file is part of cagliostro.
Copyright (C) 2020 Christopher Gundler <christopher@gundler.de>

This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>.
*/

#ifndef CAGLIOSTRO_CAGLIOSTRO_VIEW_UTIL_ASPECTRATIOLAYOUT_H_
#define CAGLIOSTRO_CAGLIOSTRO_VIEW_UTIL_ASPECTRATIOLAYOUT_H_

#include <QLayout>

namespace cagliostro::view::util {
class AspectRatioLayout : public QLayout {
 public:
  explicit AspectRatioLayout(double aspect_ratio, QWidget *parent = nullptr);
  ~AspectRatioLayout() override;
  void addItem(QLayoutItem *item) override;
  void setGeometry(const QRect &rect) override;

  [[nodiscard]] int count() const override;
  [[nodiscard]] QLayoutItem *itemAt(int i) const override;
  [[nodiscard]] QLayoutItem *takeAt(int) override;
  [[nodiscard]] Qt::Orientations expandingDirections() const override;
  [[nodiscard]] bool hasHeightForWidth() const override;
  [[nodiscard]] int heightForWidth(int width) const override;
  [[nodiscard]] QSize sizeHint() const override;
  [[nodiscard]] QSize minimumSize() const override;

 private:
  QLayoutItem *content_;
  double aspect_ratio_;
};
}

#endif //CAGLIOSTRO_CAGLIOSTRO_VIEW_UTIL_ASPECTRATIOLAYOUT_H_
