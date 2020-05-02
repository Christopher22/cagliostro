#include <iostream>

#include "model/Wizard.h"
#include "model/Selection.h"

using namespace cagliostro;

int main() {
  auto *wizard = new model::Wizard();
  auto *page1 = new model::Page("First page", 0, wizard);
  auto *page2 = new model::Page("Second page", 1, wizard);
  new model::Selection("Your gender", {"Male", "Female"}, "gender", 0, page2);
  return 0;
}
