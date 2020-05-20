/*
This file is part of cagliostro.
Copyright (C) 2020 Christopher Gundler <christopher@gundler.de>

This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>.
*/

#ifndef CAGLIOSTRO_CAGLIOSTRO_MODEL_ENTITY_H_
#define CAGLIOSTRO_CAGLIOSTRO_MODEL_ENTITY_H_

#include <QObject>
#include <algorithm>

namespace cagliostro::model {
class Entity : public QObject {
 Q_OBJECT

 public:
  explicit Entity(const QString &name, int index = 0, QObject *parent = nullptr);

  [[nodiscard]] int index() const noexcept;
  void setIndex(int index) noexcept;
  [[nodiscard]] bool hasName() const noexcept;

  template<typename T>
  static QVector<T *> get(const QObject *parent) noexcept {
	static_assert(std::is_base_of<Entity, T>::value, "T needs to be a subclass of Entity.");
	auto children = parent->findChildren<T *>(QString(), Qt::FindDirectChildrenOnly).toVector();
	std::sort(children.begin(), children.end(), [](const Entity *e1, const Entity *e2) {
	  return e1->index_ < e2->index_;
	});
	return children;
  }

 private:
  int index_;
};
}

#endif //CAGLIOSTRO_CAGLIOSTRO_MODEL_ENTITY_H_
