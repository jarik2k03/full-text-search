#include "parser.h"

Parser::Parser(const pugi::xml_document& d) {
  ASSERT(set_data(d), "Не удалось найти конфиг");
}

Parser::Parser(const str_uset& _set, uint _min, uint _max)
    : stopwords(_set), min(_min), max(_max) {
}

bool Parser::set_data(const pugi::xml_document& d) {
  const pugi::xml_node ngram = d.child("fts").child("ngram");
  min = ngram.attribute("min").as_uint();
  max = ngram.attribute("max").as_uint();
  const pugi::xml_node stop_words =
      d.child("fts").child("ngram").child("stop_words");
  for (pugi::xml_node i : stop_words.children("word")) {
    stopwords.insert(i.text().as_string());
  }
  return false;
}

void Parser::print_config() const {
  std::cout << "Ngram min: " << min << "\nNgram max: " << max;
  for (str it : stopwords)
    std::cout << it << "\t";
  std::cout << "\n";
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
    for (int i = min; i <= size && i <= max; ++i) {
      str part_s(s, 0, i);
      pr.ngrams.insert({part_s, count});
    }
    pr.positions_count = count;
    if (size >= min)
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
    auto it = stopwords.find(current);
    if (it == stopwords.end()) {
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