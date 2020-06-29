/*
This file is part of cagliostro.
Copyright (C) 2020 Christopher Gundler <christopher@gundler.de>

This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>.
*/

#include "Config.h"
#include "content/Video.h"
#include "content/preprocessors/Header.h"
#include "content/postprocessors/Delete.h"

#include <algorithm>
#include <random>
#include <chrono>

#include <QApplication>
#include <QFont>

cagliostro::model::Config::Config(Source *data, const QDir &root_dir, QObject *parent)
	: QObject(parent), source_(data), xml_(), root_(root_dir) {
  qRegisterMetaType<cagliostro::model::Config::Error>("cagliostro::model::Config::Error");
  source_->setParent(this);
  QObject::connect(this, &Config::error, this, [&](Error, const QString &message) {
	xml_.raiseError(message);
  });
}

void cagliostro::model::Config::parse() {
  auto *data = source_->open(QIODevice::ReadOnly);
  if (data == nullptr) {
	emit this->error(Error::InputError, tr("Unable to open or decrypt the input file. Is the password wrong?"));
	return;
  }

  xml_.setDevice(data);
  if (!xml_.readNextStartElement() || xml_.name() != "cagliostro") {
	emit this->error(Error::ParserError, tr("Root element 'cagliostro' not available."));
	return;
  }

  // Add an optional due date
  const auto due_date = QDateTime::fromString(this->attribute("due"), Qt::ISODate);
  if (due_date.isValid() && due_date < QDateTime::currentDateTime()) {
	emit this->error(Error::FailedCheck, tr("This survey was already due. Please contact your supervisor."));
	return;
  }

  // Parse the participant and the result file
  const auto result_file = root_.absoluteFilePath(this->attribute("result", QString("result.tsv")));
  const auto participant = this->attribute("participant");
  if (participant.isEmpty()) {
	emit this->error(Error::ParserError, tr("Participant is not defined"));
	return;
  }

  // Allow to override the font size
  auto font_size = this->attribute("font_size", 0);
  if (font_size <= 0) {
	font_size = QApplication::font().pointSize();
  }

  // Try to create the wizard
  auto *wizard = new Wizard(
	  participant,
	  this->attribute("title", QString("cagliostro")),
	  this->attribute("complete", tr("You answered all questions already. Thank you for your participation!")),
	  font_size
  );

  auto *responses = Responses::load(result_file, wizard->participant(), wizard);
  if (responses == nullptr) {
	emit this->error(Error::OutputError, tr("Unable to create output file"));
	wizard->deleteLater();
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
						   tr("Found unexpected element '%1' in 'randomized_pages'.").arg(xml_.name()));
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
	  emit this->error(Error::ParserError, tr("Found unexpected element '%1'.").arg(xml_.name()));
	}
  }, false);  // Parse all the pages

  if (*this) {
	// This allows to pull the thread to the next thread
	wizard->moveToThread(nullptr);
	emit this->done(wizard);
  } else {
	// In case the document was corrupted and no custom error raised, do it now.
	if (xml_.error() != QXmlStreamReader::CustomError) {
	  emit this->error(Error::ParserError, "The file is corrupted.");
	}
	wizard->deleteLater();
  }
}

cagliostro::model::Page *cagliostro::model::Config::parse(cagliostro::model::Wizard *parent) {
  assert(xml_.name() == "page");

  auto id = attribute("id");
  if (id.isEmpty()) {
	this->error(Error::ParserError, tr("Found page without id"));
	return nullptr;
  } else if (used_ids_.contains(id)) {
	this->error(Error::ParserError, tr("Found id '%1' twice.").arg(id));
	return nullptr;
  }

  auto page = new Page(id, parent->pages().size(), nullptr);
  page->setNextText(this->attribute("next_button", tr("Next")));

  // Check if the page should be added. If not, parse the page nevertheless, but do not translate it into the model
  const auto *responses = parent->responses();
  if (responses != nullptr && responses->contains(page)) {
	this->parse("page", []() {}, false);
	page->deleteLater();
	return nullptr;
  }

  // Mark id as used
  used_ids_.insert(id);

  this->parse("page", [&]() {
	if (xml_.name() == "title") {
	  page->setTitle(xml_.readElementText());
	} else if (xml_.name() == "description") {
	  page->setDescription(xml_.readElementText());
	} else if (xml_.name() == "content") {
	  const auto obligatory = attribute("obligatory", false);

	  // Parse content of page
	  if (attribute("type", QString("video")) != "video") {
		emit this->error(Error::ParserError, tr("Currently, only content 'video' is supported."));
		return;
	  } else if (page->content() != nullptr) {
		emit this->error(Error::ParserError, tr("More than one content element found."));
		return;
	  }

	  // Overwrite the header, if specified
	  const QString header_change = this->attribute("header");

	  // Remove the video after playing, if specified
	  const bool clean_up = this->attribute("clean_up", false);

	  // Load the resource and try to parse it as video
	  auto *resource = new content::Resource(xml_.readElementText(), root_);

	  if (!header_change.isEmpty() && content::Header::create(header_change, resource) == nullptr) {
		resource->deleteLater();
		emit this->error(Error::ResourceError, tr("Unable to decode the header of the element with id '%1'.").arg(id));
	  }

	  // Try to load the corresponding video
	  content::Video *video = content::Video::load(resource, page);
	  if (video == nullptr) {
		resource->deleteLater();
		emit this->error(Error::ResourceError, tr("Unable to load the content of the element with id '%1'.").arg(id));
	  } else {
		// Mark this element in a way that the user can not skip it
		video->setObligatory(obligatory);

		// If the video was successfully opened and should be auto-removed, prepare that
		if (clean_up) {
		  new content::Delete(resource);
		}
	  }
	} else if (xml_.name() == "question") {
	  this->parse(page);
	} else {
	  emit this->error(Error::ParserError, tr("Found unexpected element '%1' on page '%2'.").arg(xml_.name(), id));
	}
  }, false);

  page->setParent(parent);
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

cagliostro::model::Config::operator bool() const noexcept {
  return xml_.error() == QXmlStreamReader::NoError;
}

bool cagliostro::model::Config::parse(const char *name, std::function<void()> inner_element_values, bool include_root) {
  if (xml_.name() != name || xml_.tokenType() != QXmlStreamReader::StartElement) {
	return false;
  }

  // Check for endless loops
  bool skipped_read = false;

  // Do not skip start element
  while (xml_.tokenType() != QXmlStreamReader::NoToken && *this) {
	if (xml_.tokenType() == QXmlStreamReader::EndElement && xml_.name() == name) {
	  break;
	} else if (xml_.tokenType() == QXmlStreamReader::StartElement) {
	  if (include_root || xml_.name() != name) {
		inner_element_values();
	  }
	}

	// Read next token only if inner element parse read over its end
	if (xml_.tokenType() != QXmlStreamReader::StartElement || xml_.name() == name || skipped_read) {
	  xml_.readNext();
	  skipped_read = false;
	} else {
	  skipped_read = true;
	}
  }

  return true;
}
