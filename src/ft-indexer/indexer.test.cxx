#include <ft-indexer/indexer.h>
#include <gtest/gtest.h>

static std::vector<str> main_csv = {
    "bookId,title,id",
    "3,Harry Potter and Half blood prince,2",
    "14,Princess on tiny pea,12",
    "917,Fix price work manual,3"};

GTEST_TEST(indexer, build_forward_index) {
  IndexBuilder ib({{"bookId", 0}, {"title", 1}, {"id", 2}});
  const auto index = ib.build_all(main_csv);
  const forwardmap expected = {
      {"3", {"Harry Potter and Half blood prince", "2"}},
      {"14", {"Princess on tiny pea", "12"}},
      {"917", {"Fix price work manual", "3"}}};
    
  auto i = index.first.begin(), e = expected.begin();
  while (i != index.first.end() && e != expected.end()) {
    ASSERT_STREQ(i->first.c_str(), e->first.c_str());
    ASSERT_TRUE((i->second == e->second) ? true : false);
    ++i, ++e;
  }
}

GTEST_TEST(indexer, build_inverted_index) {
  IndexBuilder ib({{"bookId", 0}, {"title", 1}, {"id", 2}});
  const auto ir = ib.build_all(main_csv);
  const invertedmap& iindex = ir.second.full_index.at(0);
  const invertedmap iexpected = {
      {"har", {1, {{3, {1, {0}}}}}},
      {"harr", {1, {{3, {1, {0}}}}}},
      {"harry", {1, {{3, {1, {0}}}}}},
      {"pot", {1, {{3, {1, {1}}}}}},
      {"pott", {1, {{3, {1, {1}}}}}},
      {"potte", {1, {{3, {1, {1}}}}}},
      {"potter", {1, {{3, {1, {1}}}}}},
      {"hal", {1, {{3, {1, {2}}}}}},
      {"half", {1, {{3, {1, {2}}}}}},
      {"blo", {1, {{3, {1, {3}}}}}},
      {"bloo", {1, {{3, {1, {3}}}}}},
      {"blood", {1, {{3, {1, {3}}}}}},
      {"pri", {3, {{3, {1, {4}}}, {14, {1, {0}}}, {917, {1, {1}}}}}},
      {"pric", {1, {{917, {1, {1}}}}}},
      {"price", {1, {{917, {1, {1}}}}}},
      {"prin", {2, {{3, {1, {4}}}, {14, {1, {0}}}}}},
      {"princ", {2, {{3, {1, {4}}}, {14, {1, {0}}}}}},
      {"prince", {2, {{3, {1, {4}}}, {14, {1, {0}}}}}},
      {"tin", {1, {{14, {1, {1}}}}}},
      {"tiny", {1, {{14, {1, {1}}}}}},
      {"pea", {1, {{14, {1, {2}}}}}},
      {"fix", {1, {{917, {1, {0}}}}}},
      {"wor", {1, {{917, {1, {2}}}}}},
      {"work", {1, {{917, {1, {2}}}}}},
      {"man", {1, {{917, {1, {3}}}}}},
      {"manu", {1, {{917, {1, {3}}}}}},
      {"manua", {1, {{917, {1, {3}}}}}},
      {"manual", {1, {{917, {1, {3}}}}}}};

  auto i = iindex.begin(), e = iexpected.begin();
  while (i != iindex.end() && e != iexpected.end()) {
    // название нграмма
    ASSERT_STREQ(i->first.c_str(), e->first.c_str());
    // кол-во всех вхождений
    EXPECT_EQ(i->second.doc_count, e->second.doc_count);
    auto i_ = i->second.map.begin(), e_ = e->second.map.begin();
    while (i_ != i->second.map.end() && e_ != e->second.map.end()) {
      EXPECT_EQ(i_->first, e_->first); // id вхождения
      // кол-во позиций в одном вхождении
      EXPECT_EQ(i_->second.pos_count, e_->second.pos_count);
      // проверка позиций
      EXPECT_TRUE((i_->second.ntoken == e_->second.ntoken) ? true : false);
      ++i_, ++e_;
    }
    ++i, ++e;
  }
}
