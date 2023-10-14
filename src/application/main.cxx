#include <commons/parser.h>
#include <cxxopts.hpp>
#include <pugixml.hpp>
#include <iostream>
#include <assert.h>
#include "../commons/abstractions.h"

using namespace cxxopts;

void print_manual();

int main(int argc, char** argv) {
  if (argc == 1) {
    print_manual();
    return 0;
  }
  bool r;

  Options opt("FullTextSearch", "Okay, fts!");
  opt.add_options()("config", "XML user settings", value<std::string>()->implicit_value("config.xml"))
  ("request", "Raw search request", value<std::string>()->implicit_value("Wikipedia"));

  ParseResult pr = opt.parse(argc, argv);

  if (pr.count("config")) {
    Parser user_config (pr["config"].as<std::string>(), "user/");
    r = user_config.read_config();
    ASSERT(r, "Не удалось найти конфиг");
    user_config.print_config();
    if (pr.count("request")) {
      std::string raw = pr["request"].as<std::string>();
      std::cout << "RawSTR: " << raw << "\n";

      user_config.exclude_punct(raw);
      user_config.exclude_stop_words(raw);
    
      std::cout << "WithoutCommaSTR: " << raw << "\n";

      ParserResult my_pr = user_config.parse(raw);
      my_pr.ngrams_traverse();
    }
  }




  return 0;
}

void print_manual() {
  printf(
      "Для измерения результата сумматора необходимо ввести: \n"
      "\t- номер аргумента в виде слова (--first, --second) \n"
      "\t- вещественное число \n");
}
