#include <commons/parser.h>
#include <gtest/gtest.h>

GTEST_TEST(parser, excluding_and_down_case) {
  str s("My favourite language is C++.");
  std::unordered_set<str> us = {"to", "is", "with"};
  Parser p(us);
  ParserResult pr = p.parse(s);
  ASSERT_STREQ(s.c_str(), "my favourite language c");
}

GTEST_TEST(parser, check_parser_result) {
  str s("My favourite language is C++.");
  std::unordered_set<str> us = {"to", "is", "with"};
  Parser p(us);
  ParserResult pr = p.parse(s);
  ASSERT_EQ(pr.positions_count, 2);
  std::unordered_map<str, char> um;
  um.insert({"fav", 0});
  um.insert({"favo", 0});
  um.insert({"favou", 0});
  um.insert({"favour", 0});
  um.insert({"lan", 1});
  um.insert({"lang", 1});
  um.insert({"langu", 1});
  um.insert({"langua", 1});
  for (auto it : pr.ngrams)
    if (um.find(it.first) == um.end())
      ASSERT_FALSE(true);
}