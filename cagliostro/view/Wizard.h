/*
This file is part of cagliostro.
Copyright (C) 2020 Christopher Gundler <christopher@gundler.de>

This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>.
*/

#ifndef CAGLIOSTRO_CAGLIOSTRO_VIEW_WIZARD_H_
#define CAGLIOSTRO_CAGLIOSTRO_VIEW_WIZARD_H_

#include "../model/Wizard.h"
#include "util/Dialog.h"

namespace cagliostro::view {
class Wizard : public util::Dialog {
 public:
  explicit Wizard(QWidget *parent = nullptr);

 private:
  void createView(model::Wizard *wizard);
  model::Wizard *wizard_;
};
}

#endif //CAGLIOSTRO_CAGLIOSTRO_VIEW_WIZARD_H_
