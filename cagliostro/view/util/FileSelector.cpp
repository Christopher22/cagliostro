/*
This file is part of cagliostro.
Copyright (C) 2020 Christopher Gundler <christopher@gundler.de>

This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>.
*/

#include "FileSelector.h"

#include <QComboBox>
#include <QPushButton>
#include <QGridLayout>
#include <QFileDialog>
#include <QDragEnterEvent>
#include <QMimeData>

namespace cagliostro::view::util {

FileSelector::FileSelector(QString description, QString filter, bool for_saving, QString root, QWidget *parent)
	: QWidget(parent),
	  path_(new QComboBox(this)),
	  root_dir_(std::move(root)),
	  description_(std::move(description)),
	  filter_(std::move(filter)),
	  for_saving_(for_saving) {

  QObject::connect(path_,
				   QOverload<int>::of(&QComboBox::currentIndexChanged),
				   this,
				   &FileSelector::_onSelectionChange);

  path_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
  path_->addItem(tr("Please select a file"));

  auto *layout = new QHBoxLayout(this);
  layout->setMargin(0);
  layout->addWidget(path_);

  // Fill the file path automatically, if there are
  auto predefined_files = this->detectDefault();
  if (predefined_files.empty()) {
	this->setAcceptDrops(true);
	auto *selector = new QPushButton(this);
	selector->setText("...");
	selector->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Preferred);
	QObject::connect(selector, &QPushButton::clicked, this, &FileSelector::selectPath);

	// Add the button to the layout
	layout->addWidget(selector);

	type_ = SelectionType::Free;
  } else if (predefined_files.size() == 1) {
	// If there is a single file, set it as mandatory
	this->setPath(predefined_files[0].absoluteFilePath());
	type_ = SelectionType::Fixed;
  } else {
	// Add all the candidates to the selection
	for (auto &file: predefined_files) {
#ifdef HIDE_CAGLIOSTRO_FILES
	  path_->setItemText(0, "");
	  this->setPath(file, true, false);
#else
	  this->setPath(file, true);
#endif
	}
	type_ = SelectionType::Selection;

#ifdef HIDE_CAGLIOSTRO_FILES
	path_->setEditable(true);
	path_->setInsertPolicy(QComboBox::NoInsert);
	path_->setStyleSheet("::drop-down{image:none}");
	QObject::connect(path_, &QComboBox::editTextChanged, this, &FileSelector::_onEditTextChange);
	new Validator(path_);
	path_->setCompleter(nullptr);
#endif
  }

  this->setLayout(layout);
}

void FileSelector::selectPath() {
  const QString file_name = for_saving_ ?
							QFileDialog::getSaveFileName(this, description_, root_dir_, filter_, nullptr, options_) :
							QFileDialog::getOpenFileName(this, description_, root_dir_, filter_, nullptr, options_);

  if (file_name.isEmpty()) {
	return;
  }

  this->setPath(file_name);
}

void FileSelector::_onSelectionChange(int index) {
  QVariant data = path_->itemData(index);
  if (data.isValid()) {
	emit pathSelected(data.toString());
  }
}

FileSelector::operator bool() const noexcept {
  return path_->itemData(path_->currentIndex()).isValid();
}

QString FileSelector::path() const {
  return path_->itemData(path_->currentIndex()).toString();
}

void FileSelector::setPath(const QFileInfo &file, bool add_only, bool remove_default) {
  if (remove_default && path_->itemData(path_->currentIndex()).isNull()) {
	path_->clear();
  }

  path_->addItem(file.baseName(), file.absoluteFilePath());
  if (!add_only) {
	path_->setCurrentIndex(path_->count() - 1);
  }
}

void FileSelector::setPath(const QString &path, bool add_only, bool remove_default) {
  this->setPath(QFileInfo(path), add_only, remove_default);
}

void FileSelector::setRoot(const QString &root) {
  root_dir_ = root;
}

QFileDialog::Options *FileSelector::operator->() {
  return &options_;
}

void FileSelector::dragEnterEvent(QDragEnterEvent *event) {
  if (event->mimeData()->hasUrls()) {
	event->acceptProposedAction();
  }
}

void FileSelector::dropEvent(QDropEvent *event) {
  if (event->mimeData()->hasUrls()) {
	const auto urls = event->mimeData()->urls();
	if (urls.size() == 1) {
	  const QFileInfo local_file(urls[0].toLocalFile());
	  if (local_file.isFile()) {
		this->setPath(local_file.absoluteFilePath());
		event->acceptProposedAction();
	  }
	}
  }
}

QList<QFileInfo> FileSelector::detectDefault() const {
  if (filter_.isEmpty()) {
	return QList<QFileInfo>();
  }
  // Get the extension from the filter
  const QRegularExpression filter_regex("[^(]+\\(([^)]+)");
  const auto match = filter_regex.match(filter_);
  if (!match.isValid()) {
	return QList<QFileInfo>();
  }

  // Query for such a file
  QDir current_dir = QDir::currentPath();
  return current_dir.entryInfoList(QStringList(match.captured(1)), QDir::Files | QDir::Readable);
}

FileSelector::SelectionType FileSelector::selectionType() const noexcept {
  return type_;
}

#ifdef HIDE_CAGLIOSTRO_FILES
void FileSelector::_onEditTextChange(const QString &text) {
  auto model = qobject_cast<QStandardItemModel *>(path_->model());
  auto selection = model->findItems(text, Qt::MatchExactly);
  if (!selection.empty()) {
	this->_onSelectionChange(selection[0]->row());
  } else {
	// Inform the interested subscriber that there is no longer a valid path
	emit pathSelected("");
  }
}

FileSelector::Validator::Validator(QComboBox *parent)
	: QValidator(parent), model_(qobject_cast<QStandardItemModel *>(parent->model())) {
  assert(parent != nullptr);
  assert(model_ != nullptr);
  parent->setValidator(this);
}

QValidator::State FileSelector::Validator::validate(QString &string, int &i) const {
  auto candidates = model_->findItems(string, Qt::MatchStartsWith);
  if (candidates.empty()) {
	return QValidator::State::Invalid;
  } else if (candidates.size() == 1 && candidates[0]->text().compare(string) == 0) {
	return QValidator::State::Acceptable;
  } else {
	return QValidator::State::Intermediate;
  }
}
#endif
}