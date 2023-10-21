#pragma once
#include <pugixml.hpp>

#include <inttypes.h>
#include <algorithm>
#include <iostream>
#include <sstream>
#include <unordered_map>
#include <unordered_set>

#include <commons/abstractions.h>

struct ParserResult {
  std::unordered_map<str, uint8_t> ngrams;
  uint positions_count;
  void ngrams_traverse() const;
  ParserResult() : positions_count(0) {
  }
};

class Parser {
 private:
  uint min, max; // ngram length
  str path_to_config;
  str_uset stopwords;

  void exclude_punct(str& raw_str) const;
  void exclude_stop_words(str& raw_str) const;
  void to_lower_case(str& raw_str) const;

 public:
  Parser(cstr& name, cstr& path = "user/");
  Parser(const str_uset& _set, uint _min = 3, uint _max = 6);
  void set_min_len(uint _min) {
    min = _min;
  }
  void set_max_len(uint _max) {
    max = _max;
  }
  void set_stopword_set(str_uset& _set) {
    stopwords = std::move(_set);
  }
  uint get_min_len() const {
    return min;
  }
  uint get_max_len() const {
    return max;
  }

  bool read_config(cstr& name);
  void print_config() const;
  ParserResult parse(str& raw_str);
};