#pragma once

#include <cstdint>
#include <pugixml.hpp>
#include <string.h>
#include <iostream>


class config_properties {
 private:
  uint8_t min, max; // ngram length
  std::string config_name;

 public:
  config_properties(const std::string& name); 
  ~config_properties();
  uint8_t get_min_len() {return min;}
  uint8_t get_max_len() {return max;}
  bool read_properties();

};