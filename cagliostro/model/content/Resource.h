//
// Created by Christopher Gundler on 16.05.2020.
//

#ifndef CAGLIOSTRO_CAGLIOSTRO_MODEL_CONTENT_RESOURCE_H_
#define CAGLIOSTRO_CAGLIOSTRO_MODEL_CONTENT_RESOURCE_H_

#include <QObject>
#include <QUrl>
#include <QDir>

namespace cagliostro::model::content {
class Resource : public QObject {
 Q_OBJECT

 public:
  explicit Resource(QUrl uri, QObject *parent = nullptr);
  Resource(const QString &uri, const QDir &root_dir, QObject *parent = nullptr);
  ~Resource() override;

  [[nodiscard]] QUrl uri() noexcept;
  void close() noexcept;
  [[nodiscard]] bool isOpened() const noexcept;

  bool update(QUrl uri);

 signals:
  void prepare(Resource *resource);
  void cleanUp(Resource *resource);

 private:
  enum class Status {
    Uninitialized,
    Opening,
    Open,
    Closing,
    Closed
  };

  QUrl uri_;
  Status status_;
};
}

#endif //CAGLIOSTRO_CAGLIOSTRO_MODEL_CONTENT_RESOURCE_H_
