/*
This file is part of cagliostro.
Copyright (C) 2020 Christopher Gundler <christopher@gundler.de>

This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>.
*/

#ifndef CAGLIOSTRO_CAGLIOSTRO_VIEW_UTIL_DIALOGPAGE_H_
#define CAGLIOSTRO_CAGLIOSTRO_VIEW_UTIL_DIALOGPAGE_H_

#include "Dialog.h"
#include "ButtonGroup.h"

namespace cagliostro::view::util {
class DialogPage : public QWidget {
 Q_OBJECT

 public:
  explicit DialogPage(Dialog *dialog);

 protected:
  ButtonGroup *buttons() noexcept;
  virtual void prepare() {};
  virtual bool cleanUp() { return true; };

 signals:
  void ready(bool);

 private:
  void handleEnterSignal(int id);
  void handleLeaveSignal(int id);

  Dialog *dialog_;
  int page_id_;
};
}

#endif //CAGLIOSTRO_CAGLIOSTRO_VIEW_UTIL_DIALOGPAGE_H_
