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
      "book",
      "Raw search request",
      value<str>()->implicit_value("book.csv"));

  const ParseResult pr = opt.parse(argc, argv);
  if (pr.count("config")) {
    if (pr.count("book")) {
      str raw = pr["book"].as<str>();
      cstr config_name = pr["config"].as<str>();
      IndexBuilder index(raw, config_name);
      index.print_results();
    }
  }
  return 0;
}

int print_manual() {
  std::cout << "--config - пользовательские настройки (default:config.xml)\n"
            << "--book - проиндексировать базу книг\n";
  return 0;
}
