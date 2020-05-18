/*
This file is part of cagliostro.
Copyright (C) 2020 Christopher Gundler <christopher@gundler.de>

This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>.
*/

#include "Config.h"
#include "content/Video.h"

#include <algorithm>
#include <random>
#include <chrono>

cagliostro::model::Config::Config(QObject *parent) : QObject(parent) {
  QObject::connect(this, &Config::error, this, [&](Error, const QString &message) {
    xml_.raiseError(message);
  });
}

bool cagliostro::model::Config::parse(QIODevice *data, QDir root_dir) {
  xml_.setDevice(data);
  root_ = std::move(root_dir);

  if (xml_.readNextStartElement() && xml_.name() == "cagliostro") {
    this->parse();
    return static_cast<bool>(*this);
  } else {
    emit this->error(Error::ParserError, "Root element 'cagliostro' not available.");
  }
  return false;
}

void cagliostro::model::Config::parse() {
  assert(xml_.name() == "cagliostro");

  // Parse the participant and the result file
  const auto result_file = root_.absoluteFilePath(this->attribute("result", "result.tsv"));
  const auto participant = this->attribute("participant");
  if (participant.isEmpty()) {
    emit this->error(Error::ParserError, "Participant is not defined");
    return;
  }

  // Try to create the wizard
  auto *wizard = Wizard::load(
      participant,
      result_file,
      this->attribute("title", "cagliostro")
  );
  if (wizard == nullptr) {
    emit this->error(Error::OutputError, "Unable to create output file");
    return;
  }

  this->parse("cagliostro", [&]() {
    if (xml_.name() == "page") {
      this->parse(wizard);
    } else if (xml_.name() == "randomized_pages") {
      QVector<Page *> pages;
      this->parse("randomized_pages", [&]() {
        if (xml_.name() == "page") {
          auto *page = this->parse(wizard);
          if (page != nullptr) {
            pages.append(page);
          }
        } else {
          emit this->error(Error::ParserError,
                           QString("Found unexpected element '%1' in 'randomized_pages'.").arg(xml_.name()));
        }
      }, false);

      // Get the indices of all added pages...
      QVector<int> page_ids(pages.size());
      for (int i = 0; i < pages.size(); ++i) {
        page_ids[i] = pages[i]->index();
      }

      // ... shuffle them ...
      const auto seed = std::chrono::system_clock::now().time_since_epoch().count();
      std::shuffle(page_ids.begin(), page_ids.end(), std::default_random_engine(seed));

      // ... and re-assign.
      for (int i = 0; i < pages.size(); ++i) {
        pages[i]->setIndex(page_ids[i]);
      }
    } else {
      emit this->error(Error::ParserError, QString("Found unexpected element '%1'.").arg(xml_.name()));
    }
  }, false);  // Parse all the pages

  if (*this) {
    emit this->done(wizard);
  } else {
    wizard->deleteLater();
  }
}

cagliostro::model::Page *cagliostro::model::Config::parse(cagliostro::model::Wizard *parent) {
  assert(xml_.name() == "page");

  // ToDo: Check id is unique
  auto id = attribute("id");
  if (id.isEmpty()) {
    this->error(Error::ParserError, QString("Found page without id"));
    return nullptr;
  }

  auto page = new Page(id, parent->pages().size(), parent);
  page->setNextText(this->attribute("next_button", "Next"));

  this->parse("page", [&]() {
    if (xml_.name() == "title") {
      page->setTitle(xml_.readElementText());
    } else if (xml_.name() == "description") {
      page->setDescription(xml_.readElementText());
    } else if (xml_.name() == "content") {
      // Parse content of page
      if (attribute("type", "video") != "video") {
        emit this->error(Error::ParserError, tr("Currently, only content 'video' is supported."));
        return;
      } else if (page->content() != nullptr) {
        emit this->error(Error::ParserError, tr("More than one content element found."));
        return;
      }

      // Load the resource and try to parse it as video
      auto *resource = new content::Resource(xml_.readElementText());
      if (content::Video::load(resource, page) == nullptr) {
        resource->deleteLater();
        emit this->error(Error::ResourceError, tr("Unable to load the content of the element with id '%1'.").arg(id));
      }
    } else if (xml_.name() == "question") {
      this->parse(page);
    } else {
      emit this->error(Error::ParserError, tr("Found unexpected element '%1' on page '%2'.").arg(xml_.name(), id));
    }
  }, false);

  return page;
}

cagliostro::model::Question *cagliostro::model::Config::parse(cagliostro::model::Page *parent) {
  const int question_num = parent->questions().size();
  QStringList elements;
  auto *question = new Selection("", elements, "", question_num, parent);

  const auto parsed = this->parse("question", [&]() {
    // Parse questions
    if (xml_.name() == "question") {
      const auto question_id = this->attribute("id");
      if (question_id.isEmpty()) {
        emit this->error(Error::ParserError,
                         tr("Question %1 on page '%2' does not have a id.")
                             .arg(QString::number(question_num + 1), parent->objectName()));
        return;
      }
      question->setObjectName(question_id);
    } else if (xml_.name() == "description") {
      if (!question->text().isEmpty()) {
        emit this->error(Error::ParserError,
                         tr("More than one description found for question '%1' on page '%2'.")
                             .arg(question->objectName(), parent->objectName()));
        return;
      }
      question->setText(xml_.readElementText());
    } else if (this->parse("choices", [&]() {
      if (xml_.name() == "choice") {
        elements.append(xml_.readElementText());
      } else {
        emit this->error(Error::ParserError,
                         tr("Unknown element '%1' found in choices for question %2.").arg(xml_.name(), question_num));
      }
    }, false)) {
      if (elements.empty()) {
        emit this->error(Error::ParserError, tr("Choices of question %1 are empty.").arg(question_num));
        return;
      } else if ((*question)->rowCount() != 0) {
        emit this->error(Error::ParserError,
                         tr("Choices of question %1 were already filled.").arg(question_num));
        return;
      }
      (*question)->setStringList(elements);
    }
  });

  assert(parsed);
  return question;
}

QString cagliostro::model::Config::attribute(const char *name, const char *default_value) {
  QStringRef attribute_value = xml_.attributes().value(name);
  if (attribute_value.isEmpty() && default_value != nullptr) {
    return QString(default_value);
  } else {
    return attribute_value.toString();
  }
}

cagliostro::model::Config::operator bool() const noexcept {
  return xml_.error() == QXmlStreamReader::NoError;
}

bool cagliostro::model::Config::parse(const char *name, std::function<void()> inner_element_values, bool include_root) {
  if (xml_.name() != name || xml_.tokenType() != QXmlStreamReader::StartElement) {
    return false;
  }

  // Do not skip start element,
  while (xml_.tokenType() != QXmlStreamReader::NoToken && *this) {
    if (xml_.tokenType() == QXmlStreamReader::EndElement && xml_.name() == name) {
      break;
    } else if (xml_.tokenType() == QXmlStreamReader::StartElement) {
      if (include_root || xml_.name() != name) {
        inner_element_values();
      }
    }

    // Read next token only if inner element parse read over its end
    if (xml_.tokenType() != QXmlStreamReader::StartElement || xml_.name() == name) {
      xml_.readNext();
    }
  }

  return true;
}