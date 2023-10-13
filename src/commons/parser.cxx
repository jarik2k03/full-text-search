#include "parser.h"



Parser::Parser(cstr& name, cstr& path) : config_name(name), path_to_config(path) {  }

bool Parser::read_config() {
  
  if(config_name.empty() || path_to_config.empty()) return true;
  std::string docpath = path_to_config + config_name;
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
    stopwords.insert((char*)i.text().as_string());
    //std::cout << word.as_string() << "\n"; 
  }
  return false;
}

void Parser::print_config() {
  std::cout << "Ngram min: " << min << "\nNgram max: " << max << "\nConfig: " << path_to_config << config_name << "\n";
  for (char* it : stopwords) std::cout << it <<"\t";
  std::cout << "\n";

}



Parser& Parser::operator=(Parser&& old) {
  std::cout << "Rval -> выходной вариант с получением по ссылке" << "\n";
  if (this == &old) return *this; // предотвращение самоопределения

  stopwords.clear();
  config_name.clear();
  path_to_config.clear();

  EXCHANGE(old.min, min);
  EXCHANGE(old.max, max);
  PEXCHANGE(old.config_name, config_name);
  PEXCHANGE(old.path_to_config, path_to_config);

  return *this;

}