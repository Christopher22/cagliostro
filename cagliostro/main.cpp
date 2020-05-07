/*
This file is part of cagliostro.
Copyright (C) 2020 Christopher Gundler <christopher@gundler.de>

This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>.
*/

#include "model/Wizard.h"
#include "model/Selection.h"
#include "model/content/Video.h"
#include "view/Wizard.h"

#include <QApplication>

using namespace cagliostro;

int main(int argc, char *argv[]) {
  QApplication app(argc, argv);
  QApplication::setOrganizationName("Christopher Gundler");
  QApplication::setApplicationName("cagliostro");

  // Create the model
  auto *wizard = new model::Wizard("A Quokka survey");
  auto *page1 = new model::Page("First page", 0, wizard);
  page1->setTitle("General information for participant");
  page1->setDescription("I solemnly swear that I'm a **Quokka** lover (and have seen the *amazing* Markdown).");
  page1->setNextText("I swear!");

  auto *page2 = new model::Page("Second page", 1, wizard);
  page2->setDescription("Please provide some details about yourself:");
  new model::Selection("Your gender: ", {"Male", "Female"}, "gender", 0, page2);

  auto *page3 = new model::Page("Third page", 2, wizard);
  page3->setDescription("Please rate the cuteness of this Quokka:");
  auto *video = model::content::Video::load(QUrl::fromLocalFile("quokka.wmv"), page3);
  if (video == nullptr) {
	return 1;
  }
  new model::Selection("Cuteness: ", {"Extreme cute", "Super cute", "Cute"}, "cuteness", 0, page3);

  // View the model and run the application
  (new view::Wizard(wizard))->show();
  return QApplication::exec();
}
