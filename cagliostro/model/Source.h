/*
This file is part of cagliostro.
Copyright (C) 2020 Christopher Gundler <christopher@gundler.de>

This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>.
*/

#ifndef CAGLIOSTRO_CAGLIOSTRO_MODEL_SOURCE_H_
#define CAGLIOSTRO_CAGLIOSTRO_MODEL_SOURCE_H_

#include <QByteArray>
#include <QIODevice>

#include <functional>

namespace cagliostro::model {
class Source : public QObject {
 public:
  explicit Source(QIODevice *device, QObject *parent = nullptr);
  explicit Source(const QString &file, QObject *parent = nullptr);
  void setPassword(QStringView password) noexcept;

  [[nodiscard]] QIODevice *open(QIODevice::OpenMode mode);
  static QByteArray encrypt(QIODevice *source, QStringView password);

 private:
  QIODevice *source_;
  QByteArray password_;

  static QIODevice *generate_key_iv(const QByteArray &password,
                                    std::function<QIODevice *(unsigned char *, unsigned char *, unsigned int)> callback,
                                    unsigned int iterations = 22442);
  [[nodiscard]] QIODevice *decrypt(QIODevice::OpenMode mode);
  [[nodiscard]] static bool isCagliostroHeader(const QByteArray &header) noexcept;
};
}

#endif //CAGLIOSTRO_CAGLIOSTRO_MODEL_SOURCE_H_
