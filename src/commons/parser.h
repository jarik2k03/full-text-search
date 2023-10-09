#pragma once

#include <cstdint>
#include <pugixml.hpp>
#include <string.h>
#include <iostream>


class config_properties {
 private:
  uint min, max; // ngram length
  std::string config_name;
  std::string path_to_config;

 public:
  config_properties(const std::string& name, const std::string& path); 
  int get_min_len() {return min;}
  int get_max_len() {return max;}
  bool read_properties();
  void print_properties();

};