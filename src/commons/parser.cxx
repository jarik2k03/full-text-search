#include "parser.h"

#define STOPWORDNODE 

config_properties::config_properties(const std::string& name) {

  char* s = const_cast<char*>(name.c_str());
  config_name.copy(s, name.capacity());
}

config_properties::~config_properties() {
  
}

bool config_properties::read_properties() {
  pugi::xml_document doc;
  pugi::xml_parse_result parsed = doc.load_file(config_name.c_str());
  if (!parsed)
    return false;

  for (pugi::xml_node stop_words: doc.child("fts").child("ngram").child("stop_words").children("words"))
  {
    
  }
  

  return true;

}