/*
This file is part of cagliostro.
Copyright (C) 2020 Christopher Gundler <christopher@gundler.de>

This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>.
*/

#ifndef CAGLIOSTRO_CAGLIOSTRO_VIEW_PAGE_H_
#define CAGLIOSTRO_CAGLIOSTRO_VIEW_PAGE_H_

#include "VideoViewer.h"
#include "util/DialogPage.h"
#include "../model/Page.h"
#include "../model/content/Content.h"

#include <QVector>
#include <QFormLayout>

namespace cagliostro::view {
class Page : public util::DialogPage {
 public:
  explicit Page(model::Page *page, util::Dialog *parent = nullptr);

 protected:
  void prepare() override;
  void present() override;
  bool cleanUp() override;

 private:
  void checkReady();
  QWidget *createQuestionWidget(model::Question *question) noexcept;
  VideoViewer *createContentWidget(model::content::Content *question) noexcept;

  model::Page *page_;
  VideoViewer *video_;
  QFormLayout *question_layout_;
};
}

#endif //CAGLIOSTRO_CAGLIOSTRO_VIEW_PAGE_H_
