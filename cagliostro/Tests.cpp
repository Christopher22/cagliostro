/*
This file is part of cagliostro.
Copyright (C) 2020 Christopher Gundler <christopher@gundler.de>

This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>.
*/

#include "Tests.h"

#include "model/Responses.h"
#include "model/Selection.h"

void Tests::testResponses() {
  QByteArray example_content
	  ("DATETIME\tSUBJECT\tQUESTION (%1)\tANSWER\n2017-07-24T15:46:29\te1\tpage1/question1\t42\n2017-07-24T15:47:29\te1\tpage2/question1\t42\n2018-07-24T15:47:29\te1\tpage2/question2\t42\n");
  auto *buffer = new QBuffer(&example_content, this);
  buffer->open(QIODevice::ReadWrite | QIODevice::Text | QIODevice::Append);
  auto *responses = new cagliostro::model::Responses(buffer, "e1", this);

  QVERIFY(responses->contains(QString("page2")));
  QVERIFY(!responses->contains(QString("page3")));

  auto parsed_responses = responses->responses();
  QCOMPARE(parsed_responses.size(), 3);
  QCOMPARE(parsed_responses[1]->objectName(), "page2/question1");

  auto *page = new cagliostro::model::Page("page3");
  auto *question = new cagliostro::model::Selection("Is 42 the answer to everything?",
													{"Yes", "Unsure, but I believe it"},
													"is42",
													0,
													page);

  QVERIFY(!responses->contains(page));
  QVERIFY(!responses->contains(question));

  // Ensure unanswered questions can not be logged
  QVERIFY(!responses->record(page));
  QVERIFY(!responses->record(question));

  question->setAnswer(0);
  QVERIFY(responses->record(question));

  // Ensure a question can only be answered once
  QVERIFY(!responses->record(page));
  QVERIFY(!responses->record(question));

  QVERIFY(responses->contains(QString("page3")));
  QVERIFY(responses->contains(page));
  QVERIFY(responses->contains(question));
}
