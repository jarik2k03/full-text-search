#include <cxxopts.hpp>
#include <pugixml.hpp>

#include <iostream>

#include <commons/abstractions.h>
#include <commons/parser.h>
#include <ft-indexer/indexer.h>

using namespace cxxopts;

int print_manual();

int main(int argc, char** argv) {
  if (argc == 1)
    return print_manual();

  Options opt("FullTextSearch", "Okay, fts!");
  opt.add_options()(
      "config",
      "XML user settings",
      value<str>()->implicit_value("config.xml"))(
      "request",
      "Raw search request",
      value<str>()->implicit_value("Wikipedia"));

  const ParseResult pr = opt.parse(argc, argv);
  if (pr.count("config")) {
    if (pr.count("request")) { 
      str raw = pr["request"].as<str>();
      cstr config_name = pr["config"].as<str>();
      IndexBuilder index(config_name);
      index.add_document(396087, raw);
      index.add_document(22305, "Zyravek kryor!");
      index.print_results();
    }

  }
  return 0;
}

int print_manual() {
  std::cout << "--config - пользовательские настройки (default:config.xml)\n"
            << "--request - пользовательский запрос\n";
  return 0;
}
