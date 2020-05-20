/*
This file is part of cagliostro.
Copyright (C) 2020 Christopher Gundler <christopher@gundler.de>

This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>.
*/

#ifndef CAGLIOSTRO_CAGLIOSTRO_VIEW_CONFIGPAGE_H_
#define CAGLIOSTRO_CAGLIOSTRO_VIEW_CONFIGPAGE_H_

#include "util/FileSelector.h"
#include "../model/Wizard.h"
#include "../model/Config.h"

#include <QWizardPage>
#include <QThread>

namespace cagliostro::view {
class ConfigPage : public QWizardPage {
 Q_OBJECT
  Q_PROPERTY(model::Wizard *model READ model NOTIFY modelLoaded);

 public:
  explicit ConfigPage(QWizard *parent = nullptr);
  void initializePage() override;
  bool validatePage() override;
  [[nodiscard]] bool isComplete() const override;
  [[nodiscard]] inline model::Wizard *model() const noexcept {
	return model_;
  }

 signals:
  void modelLoaded(model::Wizard *model);
  void loadingStarted();

 private:
  QThread *worker_;
  util::FileSelector *file_selector_;
  model::Wizard *model_;

  void setModel(model::Wizard *model);
  void stopWorker();
  bool startWorker();
  void handleError(model::Config::Error error, const QString &error_message);
};
}

#endif //CAGLIOSTRO_CAGLIOSTRO_VIEW_CONFIGPAGE_H_
