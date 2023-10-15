#include <commons/parser.h>
#include <gtest/gtest.h>

GTEST_TEST(parser, read_config) {
  Parser p("config.xml", "NULL/");
  bool err = p.read_config();
  ASSERT_TRUE(err);

  Parser p1("config.xml", "user/");
  err = p.read_config();
  ASSERT_TRUE(err);

  Parser p2("NULL.xml", "user/");
  err = p.read_config();
  ASSERT_TRUE(err);
}

GTEST_TEST(parser, excluding_and_down_case) {
  Parser p("config.xml", "user/");
  bool err = p.read_config();
  str s("My favourite language is C++.");
  p.exclude_punct(s);
  ASSERT_STREQ(s.c_str(), "My favourite language is C");

  p.to_lower_case(s);
  ASSERT_STREQ(s.c_str(), "my favourite language is c");

  p.exclude_stop_words(s);
  ASSERT_STREQ(s.c_str(), "my favourite language c");
}

GTEST_TEST(parser, check_parser_result) {
  Parser p("config.xml", "user/");
  bool err = p.read_config();
  ASSERT_FALSE(err);

  str s("My favourite language is C++.");
  p.exclude_punct(s);
  p.to_lower_case(s);
  p.exclude_stop_words(s);

  ParserResult pr(0);
  pr = p.parse(s);
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