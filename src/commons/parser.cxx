#include "parser.h"

config_properties::config_properties(const std::string& name, const std::string& path) {

  config_name = name;
  path_to_config = path;
}

bool config_properties::read_properties() {
  
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

void config_properties::print_properties() {
  std::cout << "Ngram min: " << min << "\nNgram max: " << max << "\nConfig: " << path_to_config << config_name << "\n";
  for (char* it : stopwords) std::cout << it <<"\t";
  std::cout << "\n";

}