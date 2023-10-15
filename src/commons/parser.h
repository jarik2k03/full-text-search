#pragma once
#include <pugixml.hpp>

#include <algorithm>
#include <iostream>
#include <sstream>
#include <unordered_map>
#include <unordered_set>

#include <commons/abstractions.h>

struct ParserResult {
  std::unordered_map<str, char> ngrams;
  uint positions_count;
  void ngrams_traverse();
  ParserResult(uint count) : positions_count(count) {
  }
};

class Parser {
 private:
  uint min, max; // ngram length
  str config_name;
  str path_to_config;
  std::unordered_set<str> stopwords;

 public:
  Parser(cstr& name, cstr& path);

  void exclude_punct(str& raw_str);
  void exclude_stop_words(str& raw_str);
  void to_lower_case(str& raw_str);

  uint get_min_len() {
    return min;
  }
  uint get_max_len() {
    return max;
  }

  bool read_config();
  void print_config();
  ParserResult parse(cstr& raw_str);
};