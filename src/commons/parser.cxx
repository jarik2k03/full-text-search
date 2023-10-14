#include "parser.h"



Parser::Parser(cstr& name, cstr& path) : config_name(name), path_to_config(path) {  }

bool Parser::read_config() {
  
  if(config_name.empty() || path_to_config.empty()) return true;
  str docpath = path_to_config + config_name;
  pugi::xml_document doc;
  pugi::xml_parse_result parsed = doc.load_file(docpath.c_str());
  if (!parsed)
    return true;

  pugi::xml_node ngram = doc.child("fts").child("ngram");
  min = ngram.attribute("min").as_uint();
  max = ngram.attribute("max").as_uint();

  pugi::xml_node stop_words = doc.child("fts").child("ngram").child("stop_words");

  for (pugi::xml_node i: stop_words.children("word"))
  {
    stopwords.insert(i.text().as_string());
    //std::cout << word.as_string() << "\n"; 
  }
  return false;
}

void Parser::print_config() {
  std::cout << "Ngram min: " << min << "\nNgram max: " << max << "\nConfig: " << path_to_config << config_name << "\n";
  for (str it : stopwords) std::cout << it <<"\t";
  std::cout << "\n";

}

ParserResult Parser::parse(cstr& raw_str) {
  ParserResult pr(0);
  std::stringstream ss(raw_str);
  str s;
  int count = 0;


  for ( ; ss >> s; ++count)
  {
    char size = s.size();
    for (int i = min; i <= size && i <= max; ++i)
    {
      str part_s(s, 0, i);
      pr.ngrams.insert({part_s, count});

    }
    pr.positions_count = count;
  }
   
  return pr;
}

void Parser::exclude_punct(str& raw_str) {

  auto it = std::remove_if(raw_str.begin(), raw_str.end(), ::ispunct);
  raw_str.erase(it, raw_str.end());

}

void Parser::exclude_stop_words(str& raw_str) {
  
  std::stringstream ss(raw_str);
  str cur, stop;

  // while (ss >> cur) {
  //   stop = stopwords["c"];
    
  // }  
}

void ParserResult::ngrams_traverse() {

  for (const auto& [str, c] : ngrams) std::cout << "NGRAM: " << str << "\tPOSITION: " << (int)c <<"\n";

}