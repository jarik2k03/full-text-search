#pragma once

#include <pugixml.hpp>
#include <string.h>
#include <iostream>
#include <unordered_set>
#include <unordered_map>

#include "abstractions.h"



struct ParserResult
{
  std::unordered_map<char*, char> ngrams;
  uint positions_count;
  
};

class Parser {
 private:
  uint min, max; // ngram length
  std::string config_name;
  std::string path_to_config;
  std::unordered_set<char*> stopwords;
  void clear_comma();

 public:
  Parser(cstr& name, cstr& path);
  Parser& operator=(Parser&& old); 

  int get_min_len() {return min;}
  int get_max_len() {return max;}
  
  bool read_config();
  void print_config();
  bool parse(cstr& raw_str);
};