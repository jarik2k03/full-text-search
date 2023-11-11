#pragma once
#ifndef CONFIGURATOR_H
#define CONFIGURATOR_H

#include <commons/abstractions.h>
#include <pugixml.hpp>

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