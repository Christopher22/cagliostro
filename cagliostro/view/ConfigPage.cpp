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

namespace cagliostro::view {

ConfigPage::ConfigPage(QWizard *parent)
	: QWizardPage(parent),
	  worker_(new QThread(this)),
	  model_(nullptr),
	  file_selector_(new util::FileSelector(tr("Please select the survey"), "", false, "", this)) {

  this->setTitle(tr("Please specify your input"));
  this->setSubTitle(
	  tr("Please specify the file to the survey you recieved from your supervisors."));
  this->setCommitPage(true);

  QObject::connect(this, &QObject::destroyed, this, &ConfigPage::stopWorker);
  QObject::connect(file_selector_, &util::FileSelector::pathSelected, this, [this](const QString &path) {
	QAbstractButton *custom_button;
	if (this->wizard() != nullptr && (custom_button = this->wizard()->button(QWizard::CustomButton1)) != nullptr) {
	  custom_button->setEnabled(!path.isEmpty());
	}
  });

  auto *layout = new QFormLayout();
  layout->addRow(tr("File"), file_selector_);
  this->setLayout(layout);
}

void ConfigPage::initializePage() {
  QAbstractButton *custom_button;
  if (this->wizard() != nullptr && (custom_button = this->wizard()->button(QWizard::CustomButton1)) != nullptr) {
	custom_button->setEnabled(false);
	custom_button->setVisible(true);
	custom_button->setText(tr("Load"));
	QObject::connect(custom_button, &QAbstractButton::clicked, this, [this]() {
	  if (!this->startWorker()) {
		QMessageBox::warning(this, tr("Invalid input"), tr("Cagliostro was unable to open the requested file."));
	  }
	});
  }

  QWizardPage::initializePage();
}

bool ConfigPage::isComplete() const {
  return model_ != nullptr;
}

void ConfigPage::setModel(model::Wizard *model) {
  if (model == model_ || model->thread() != nullptr) {
	return;
  }

  model->moveToThread(this->thread());
  model_ = model;
  emit this->modelLoaded(model);
  emit this->completeChanged();

  this->setFinalPage(false);
  this->wizard()->button(QWizard::CommitButton)->setText("Start");
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
  auto *file = new QFile(file_info.absoluteFilePath());
  if (!file->open(QIODevice::ReadOnly)) {
	file->deleteLater();
	return false;
  }

  file_selector_->setEnabled(false);
  this->wizard()->button(QWizard::CustomButton1)->setEnabled(false);

  auto *config = new model::Config(file, file_info.absolutePath(), nullptr);
  file->setParent(config);
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
  this->wizard()->button(QWizard::CustomButton1)->setEnabled(true);
}

bool ConfigPage::validatePage() {
  if (!QWizardPage::validatePage()) {
	return false;
  }

  this->stopWorker();
  this->wizard()->button(QWizard::CustomButton1)->setVisible(false);

  return true;
}
}