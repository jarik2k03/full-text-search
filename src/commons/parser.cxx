#include "parser.h"

Parser::Parser(cstr& name, cstr& path)
    : config_name(name), path_to_config(path) {
}

bool Parser::read_config() {
  if (config_name.empty() || path_to_config.empty())
    return true;
  str docpath = path_to_config + config_name;
  pugi::xml_document doc;
  pugi::xml_parse_result parsed = doc.load_file(docpath.c_str());
  if (!parsed)
    return true;

  pugi::xml_node ngram = doc.child("fts").child("ngram");
  min = ngram.attribute("min").as_uint();
  max = ngram.attribute("max").as_uint();

  pugi::xml_node stop_words =
      doc.child("fts").child("ngram").child("stop_words");

  for (pugi::xml_node i : stop_words.children("word")) {
    stopwords.insert(i.text().as_string());
    // std::cout << word.as_string() << "\n";
  }
  return false;
}

void Parser::print_config() {
  std::cout << "Ngram min: " << min << "\nNgram max: " << max
            << "\nConfig: " << path_to_config << config_name << "\n";
  for (str it : stopwords)
    std::cout << it << "\t";
  std::cout << "\n";
}

ParserResult Parser::parse(str& raw_str) {
  exclude_punct(raw_str);
  to_lower_case(raw_str);
  exclude_stop_words(raw_str);

  ParserResult pr(0);
  std::stringstream ss(raw_str);
  str s;
  int count = 0;

  while (ss >> s) {
    char size = s.size();
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

void Parser::exclude_punct(str& raw_str) {
  auto it = std::remove_if(raw_str.begin(), raw_str.end(), ::ispunct);
  raw_str.erase(it, raw_str.end());
}

void Parser::exclude_stop_words(str& raw_str) {
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

void Parser::to_lower_case(str& raw_str) {
  std::transform(raw_str.cbegin(), raw_str.cend(), raw_str.begin(), ::tolower);
}

void ParserResult::ngrams_traverse() {
  for (const auto& [str, c] : ngrams)
    std::cout << "NGRAM: " << str << "\tPOSITION: " << (int)c << "\n";
}