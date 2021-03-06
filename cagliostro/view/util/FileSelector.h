/*
This file is part of cagliostro.
Copyright (C) 2020 Christopher Gundler <christopher@gundler.de>

This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>.
*/

#ifndef CAGLIOSTRO_CAGLIOSTRO_VIEW_UTIL_FILESELECTOR_H_
#define CAGLIOSTRO_CAGLIOSTRO_VIEW_UTIL_FILESELECTOR_H_

#include <QWidget>
#include <QFileDialog>
#ifdef HIDE_CAGLIOSTRO_FILES
#include <QValidator>
#include <QStandardItemModel>
#endif

class QComboBox;

namespace cagliostro::view::util {
class FileSelector : public QWidget {
 Q_OBJECT

 public:
  Q_PROPERTY(QString path
				 READ
					 path
				 WRITE
					 setPath
				 NOTIFY
				 pathSelected)

  enum class SelectionType {
	Fixed,
	Selection,
	Free
  };

  FileSelector(QString description,
			   QString filter,
			   bool for_saving = false,
			   QString root = QString(),
			   QWidget *parent = nullptr);
  [[nodiscard]] QString path() const;

  void setPath(const QFileInfo &file, bool add_only = false, bool remove_default = true);
  void setPath(const QString &path, bool add_only = false, bool remove_default = true);
  void setRoot(const QString &root);

  explicit operator bool() const noexcept;
  [[nodiscard]] SelectionType selectionType() const noexcept;

  QFileDialog::Options *operator->();

 signals:
  void pathSelected(const QString &path);

 protected:
  void dragEnterEvent(QDragEnterEvent *event) override;
  void dropEvent(QDropEvent *event) override;

 private:
  void selectPath();
  void _onSelectionChange(int index);
  [[nodiscard]] QList<QFileInfo> detectDefault() const;

#ifdef HIDE_CAGLIOSTRO_FILES
  class Validator : public QValidator {
   public:
	explicit Validator(QComboBox *parent);
	State validate(QString &string, int &i) const override;

   private:
	QStandardItemModel *model_;
  };
  void _onEditTextChange(const QString &text);
#endif

  QComboBox *path_;
  QString root_dir_;
  const QString description_, filter_;
  const bool for_saving_;
  QFileDialog::Options options_;
  SelectionType type_;
};
}

#endif //CAGLIOSTRO_CAGLIOSTRO_VIEW_UTIL_FILESELECTOR_H_
