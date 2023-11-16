#include "parser.h"

Parser::Parser(const ParserOpts& po) : options(po) {
}

Parser::Parser(const str_uset& _set, uint _min, uint _max) {
  options.max_length = _max;
  options.min_length = _min;
  options.stopwords = _set;
}

ParserResult Parser::parse(str& raw_str) {
  exclude_punct(raw_str);
  to_lower_case(raw_str);
  exclude_stop_words(raw_str);

  ParserResult pr;
  std::stringstream ss(raw_str);
  str s;
  uint8_t count = 0;

  while (ss >> s) {
    int size = s.size();
    for (int i = options.min_length; i <= size && i <= options.max_length;
         ++i) {
      str part_s(s, 0, i);
      pr.ngrams.insert({part_s, count});
    }
    pr.positions_count = count;
    if (size >= options.min_length)
      ++count;
  }

  return pr;
}

void Parser::exclude_punct(str& raw_str) const {
  const auto it = std::remove_if(raw_str.begin(), raw_str.end(), ::ispunct);
  raw_str.erase(it, raw_str.end());
}

void Parser::exclude_stop_words(str& raw_str) const {
  std::stringstream ss(raw_str);
  str current;
  uint index = 0;

  while (ss >> current) {
    auto it = options.stopwords.find(current);
    if (it == options.stopwords.end()) {
      index += current.length() + 1;
      continue;
    }
    raw_str.erase(index, current.size() + 1);
  }
}

void Parser::to_lower_case(str& raw_str) const {
  std::transform(raw_str.cbegin(), raw_str.cend(), raw_str.begin(), ::tolower);
}

void ParserResult::ngrams_traverse() const {
  for (const auto& [str, c] : ngrams)
    std::cout << "NGRAM: " << str << "\tPOSITION: " << static_cast<int>(c)
              << "\n";
}