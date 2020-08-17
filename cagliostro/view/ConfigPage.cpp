/*
This file is part of cagliostro.
Copyright (C) 2020 Christopher Gundler <christopher@gundler.de>

This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>.
*/

#include "ConfigPage.h"

#include <QAbstractButton>
#include <QFormLayout>
#include <QMessageBox>
#include <QLabel>
#include <QPushButton>

namespace cagliostro::view {

ConfigPage::ConfigPage(util::Dialog *parent)
    : util::DialogPage(parent),
      worker_(new QThread(this)),
      model_(nullptr),
      file_selector_(new util::FileSelector(tr("Please select the survey"),
                                            tr("Survey (*.cagliostro)"),
                                            false,
                                            "",
                                            this)),
      password_(new QLineEdit(this)),
      load_button_id_(-1) {

  QObject::connect(this, &QObject::destroyed, this, &ConfigPage::stopWorker);
  QObject::connect(file_selector_, &util::FileSelector::pathSelected, this, &ConfigPage::setConfigPath);

  // Make the input suitable for passwords
  password_->setEchoMode(QLineEdit::Password);

  auto layout = new QVBoxLayout();
  auto *fields = new QFormLayout();

  // If the file was automatically determined, do not show the widget, but trigger the signal manually.
  if (file_selector_->selectionType() != util::FileSelector::SelectionType::Fixed) {
	layout->addWidget(new QLabel(tr("Please specify the survey you recieved from your supervisors:"),
								 this));
	fields->addRow(tr("Survey"), file_selector_);
  } else {
	layout->addWidget(new QLabel(tr("Please enter the password you recieved from your supervisors."),
								 this));
	file_selector_->setVisible(false);
  }
  fields->addRow(tr("Password"), password_);
  layout->addLayout(fields);

  this->setLayout(layout);
}

void ConfigPage::prepare() {
  // Add the load button
  auto *button = new QPushButton(tr("Load"));
  button->setEnabled(false);
  QObject::connect(button, &QAbstractButton::clicked, this, [this]() {
    if (!this->startWorker()) {
      QMessageBox::warning(this, tr("Invalid input"), tr("Cagliostro was unable to open the requested file."));
    }
  });
  load_button_id_ = this->buttons()->addButton(button);
  assert(load_button_id_ != -1);

  // If the file is already selected, allow to continue.
  if (*file_selector_) {
    this->setConfigPath(file_selector_->path());
  }
}

void ConfigPage::setModel(model::Wizard *model) {
  if (model == model_ || model->thread() != nullptr) {
    return;
  }

  model->moveToThread(this->thread());
  model_ = model;
  emit this->modelLoaded(model);
  emit this->ready(true);

  // Do not overwrite the default if no questions are present
  if (model->includeQuestions()) {
    this->buttons()->button(util::Dialog::NEXT_BUTTON)->setText(tr("Start"));
  }
}

void ConfigPage::stopWorker() {
  if (!worker_->isRunning()) {
    return;
  }

  worker_->quit();
  if (!worker_->wait(1000)) {
    qWarning("Unable to stop worker!");
  }
}

bool ConfigPage::startWorker() {
  // Get info of the file given as argument
  const QFileInfo file_info(file_selector_->path());
  if (!file_info.exists()) {
    return false;
  }

  // Disable file selection
  file_selector_->setEnabled(false);
  this->buttons()->button(load_button_id_)->setEnabled(false);

  // Create both source and config - config will take ownership
  auto *source = new model::Source(file_info.absoluteFilePath());
  if (!password_->text().isEmpty()) {
    source->setPassword(password_->text());
  }
  auto *config = new model::Config(source, file_info.absolutePath(), nullptr);

  config->moveToThread(worker_);
  QObject::connect(worker_, &QThread::destroyed, config, &model::Config::deleteLater);
  QObject::connect(config, &model::Config::done, this, &ConfigPage::setModel);
  QObject::connect(config, &model::Config::error, this, &ConfigPage::handleError);

  // Ensure last-time parser are not called again
  QObject::disconnect(this, &ConfigPage::loadingStarted, nullptr, nullptr);
  QObject::connect(this, &ConfigPage::loadingStarted, config, qOverload<>(&model::Config::parse));

  if (!worker_->isRunning()) {
    QObject::connect(worker_, &QThread::started, this, [this]() {
      emit this->loadingStarted();
    });
    worker_->start();
  } else {
    emit this->loadingStarted();
  }
  return true;
}

void ConfigPage::handleError(model::Config::Error, const QString &error_message) {
  QMessageBox::warning(this, tr("File error"), error_message);
  file_selector_->setEnabled(true);
  this->buttons()->button(load_button_id_)->setEnabled(true);
}

bool ConfigPage::cleanUp() {
  this->stopWorker();
  this->buttons()->removeButton(load_button_id_);
  return true;
}

void ConfigPage::setConfigPath(const QString &path) {
  this->buttons()->button(load_button_id_)->setEnabled(!path.isEmpty());
}
}