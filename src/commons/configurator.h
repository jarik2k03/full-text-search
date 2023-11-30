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

struct SearchState {
  str _title_request;
  std::map<str, str> _search_attrs;
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
  SearchState get_request_from_doc(cstr& filename);

  pugi::xml_document& get_document() {
    return userconfig;
  }
  IdxBuilderOpts get_builder_opts() const {
    return builder;
  }
  IdxWriterOpts get_writer_opts() const {
    return writer;
  }
  ParserOpts get_parser_opts() const {
    return parser;
  }
};

#endif