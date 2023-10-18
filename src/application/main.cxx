#include <cxxopts.hpp>
#include <pugixml.hpp>

#include <iostream>

#include <commons/abstractions.h>
#include <commons/parser.h>

using namespace cxxopts;

void print_manual();

int main(int argc, char** argv) {
  if (argc == 1) {
    print_manual();
    return 0;
  }
  bool r;

  Options opt("FullTextSearch", "Okay, fts!");
  opt.add_options()(
      "config",
      "XML user settings",
      value<std::string>()->implicit_value("config.xml"))(
      "request",
      "Raw search request",
      value<std::string>()->implicit_value("Wikipedia"));

  ParseResult pr = opt.parse(argc, argv);

  if (pr.count("config")) {
    Parser user(pr["config"].as<std::string>(), "user/");
    r = user.read_config();
    ASSERT(r, "Не удалось найти конфиг");
    if (pr.count("request")) {
      std::string raw = pr["request"].as<std::string>();

      ParserResult user_parsed = user.parse(raw);
      user_parsed.ngrams_traverse();
    }
  }
  return 0;
}

void print_manual() {
  std::cout << "--config - пользовательские настройки (default:config.xml)\n"
            << "--request - пользовательский запрос\n";
}
