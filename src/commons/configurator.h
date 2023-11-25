#pragma once
#ifndef CONFIGURATOR_H
#define CONFIGURATOR_H

#include <commons/_abstractions.h>
#include <commons/_indexstructs.h>
#include <pugixml.hpp>
#include <unordered_set>

struct ParserOpts {
  std::unordered_set<str> stopwords;
  int min_length, max_length;
};

struct IdxWriterOpts {
  int hash_length, part_length;
};

struct IdxBuilderOpts {
  booktagsvector words;
};

class Configurator {
 private:
  pugi::xml_document userconfig;
  ParserOpts parser;
  IdxWriterOpts writer;
  IdxBuilderOpts builder;

 public:
  Configurator();
  Configurator(cstr& path_to_config);

  pugi::xml_document& get_document() {
    return userconfig;
  }
  IdxBuilderOpts& get_builder_opts() {
    return builder;
  }
  IdxWriterOpts& get_writer_opts() {
    return writer;
  }
  ParserOpts& get_parser_opts() {
    return parser;
  }
};

#endif