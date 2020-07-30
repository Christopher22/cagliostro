/*
This file is part of cagliostro.
Copyright (C) 2020 Christopher Gundler <christopher@gundler.de>

This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>.
*/

#ifndef CAGLIOSTRO_CAGLIOSTRO_VIEW_UTIL_DIALOG_H_
#define CAGLIOSTRO_CAGLIOSTRO_VIEW_UTIL_DIALOG_H_

#include "ButtonGroup.h"

#include <QWidget>
#include <QStackedWidget>
#include <QMap>
#include <QAbstractButton>

namespace cagliostro::view::util {
class Dialog : public QWidget {
 Q_OBJECT

 public:
  constexpr static int NEXT_BUTTON = 0;

  explicit Dialog(QWidget *parent);
  int registerPage(QWidget *widget);
  bool setPageReady(int id, bool ready = true);
  bool abortLeave(int id);
  ButtonGroup *buttons() noexcept;
  [[nodiscard]] QSize sizeHint() const override;
  [[nodiscard]] QSize minimumSizeHint() const override;

 protected:
  virtual void prepare();
  void showEvent(QShowEvent *event) override;
  void keyPressEvent(QKeyEvent *event) override;

 signals:
  void enterPage(int id);
  void enteredPage(int id);
  void leavePage(int id);

 private:
  void nextPage();

  QStackedWidget *pages_;
  ButtonGroup *buttons_;
  bool abort_, initialized_;
};
}

#endif //CAGLIOSTRO_CAGLIOSTRO_VIEW_UTIL_DIALOG_H_
