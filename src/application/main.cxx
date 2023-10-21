#include <cxxopts.hpp>
#include <pugixml.hpp>

#include <iostream>

#include <commons/abstractions.h>
#include <commons/parser.h>

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
    Parser user(pr["config"].as<str>());
    if (pr.count("request")) {
      str raw = pr["request"].as<str>();
      const ParserResult user_parsed = user.parse(raw);
      user_parsed.ngrams_traverse();
    }
  }
  return 0;
}

int print_manual() {
  std::cout << "--config - пользовательские настройки (default:config.xml)\n"
            << "--request - пользовательский запрос\n";
  return 0;
}
