#ifndef CONFIGURATOR_H
#define CONFIGURATOR_H
#pragma once
#include <pugixml.hpp>
#include <commons/abstractions.h>

class Configurator {
 private:
 pugi::xml_document userconfig;
 
 public:
  Configurator(cstr& path_to_config);
  pugi::xml_document& get_document() {
    return userconfig;
  }
};

#endif