#pragma once

#include <cstdint>
#include <pugixml.hpp>
#include <string.h>
#include <iostream>
#include <unordered_set>


using cstr = const std::string;

struct ParserResult
{
  
};




class Parser {
 private:
  uint min, max; // ngram length
  std::string config_name;
  std::string path_to_config;
  std::unordered_set<char*> stopwords;
  

 public:
  Parser(cstr& name, cstr& path);
  
  void clear_comma();
  bool parse(cstr& raw_str);
  
  int get_min_len() {return min;}
  int get_max_len() {return max;}
  
  bool read_config();
  void print_config();

};