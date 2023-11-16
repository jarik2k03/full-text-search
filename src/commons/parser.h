#pragma once
#ifndef PARSER_H
#define PARSER_H

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
  ParserOpts options;
  void exclude_punct(str& raw_str) const;
  void exclude_stop_words(str& raw_str) const;
  void to_lower_case(str& raw_str) const;

 public:
  Parser(const ParserOpts& po);
  Parser(const str_uset& _set, uint _min = 3, uint _max = 6);

  ParserResult parse(str& raw_str);
};
#endif