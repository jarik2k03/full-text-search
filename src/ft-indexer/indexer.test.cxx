#include <ft-indexer/indexer.h>
#include <gtest/gtest.h>

std::vector<str> main_csv = {
    "bookId, title, num",
    "3, Harry Potter and Half blood prince, 0",
    "14, Princess on tiny pea, 12",
    "917, Fix price work manual, 3"};
// pri

GTEST_TEST(indexer, add_doc) {
  const booktagsvector bt = {{"bookId", 0}, {"title", 1}, {"num", 2}};
  IndexBuilder ib(bt);
  str line;
  int count = 0;
}