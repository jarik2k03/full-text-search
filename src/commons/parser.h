#ifndef PARSER_H
#define PARSER_H

#pragma once
#include <inttypes.h>
#include <algorithm>
#include <iostream>
#include <sstream>
#include <unordered_map>
#include <unordered_set>

#include <commons/configurator.h>

using str_uset = std::unordered_set<str>;
using ngram_ummap = std::unordered_multimap<str, uint8_t>;

struct ParserResult {
  ngram_ummap ngrams;
  uint positions_count;
  void ngrams_traverse() const;
  ParserResult() : positions_count(0) {
  }
};

class Parser {
 private:
  uint min, max; // ngram length
  str_uset stopwords;

  void exclude_punct(str& raw_str) const;
  void exclude_stop_words(str& raw_str) const;
  void to_lower_case(str& raw_str) const;

 public:
  Parser(const pugi::xml_document& d);
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
  bool set_data(const pugi::xml_document& d);
  void print_config() const;
  ParserResult parse(str& raw_str);
};
#endif