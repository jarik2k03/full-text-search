#include <ft-searcher/searcher.h>
#include <gtest/gtest.h>

GTEST_TEST(searcher, term_frequency) {
  InvertedIndex iindex = {3, {{3, {1, {4}}}, {14, {1, {0}}}, {917, {1, {1}}}}};
  auto result = tf("pri", 14, iindex);
  ASSERT_EQ(result, 1);

  iindex = {3, {{3, {1, {4}}}, {14, {1, {0}}}, {917, {3, {1, 3, 6}}}}};
  result = tf("pri", 917, iindex);
  ASSERT_EQ(result, 3);

  result = tf("pri", 229, iindex);
  ASSERT_EQ(result, 0);
}

GTEST_TEST(searcher, doc_frequency) {
  InvertedIndex iindex = {3, {{3, {1, {4}}}, {14, {1, {0}}}, {917, {1, {1}}}}};
  auto result = df("pri", iindex);
  ASSERT_EQ(result, 3);

  iindex = {
      5,
      {{3, {1, {4}}},
       {14, {1, {0}}},
       {917, {3, {1, 3, 6}}},
       {163, {2, {1, 2}}},
       {290, {1, {2}}}}};
  result = df("pri", iindex);
  ASSERT_EQ(result, 5);

  InvertedIndex empty;
  result = df("pri", empty);
  ASSERT_EQ(result, 0);
}

GTEST_TEST(searcher, calc_score) {
  scoreforwardmap findex = {
      {3, {{"Harry Potter and Half blood prince", "2"}, 0.0}},
      {14, {{"Princess on tiny pea", "12"}, 0.0}},
      {917, {{"Fix price work manual", "3"}, 0.0}}};

  const invertedmap iindex = {
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

  IndexProcessor ip({{"to", "is", "with", "and", "on"}, 3, 6});
  ip.set_doc_list(findex);
  ip.set_all_docs(20);
  str request("blood prince");
  const ParserResult pr = ip.get_parser().parse(request);

  ip.calc_score(pr, iindex);

  scoreforwardmap expected = {
      {3, {{"Harry Potter and Half blood prince", "2"}, 17.792072084529991}},
      {14, {{"Princess on tiny pea", "12"}, 8.8048752638680199}},
      {917, {{"Fix price work manual", "3"}, 1.8971199848858813}}};

  auto i = findex.begin(), e = expected.begin();
  while (i != findex.end() && e != expected.end()) {
    ++i, ++e;
  }
}
