#include <commons/parser.h>
#include <cxxopts.hpp>
#include <pugixml.hpp>
#include <iostream>
#include <assert.h>
#include "macros.h"

using namespace cxxopts;

void print_manual();

int main(int argc, char** argv) {
  if (argc == 1) {
    print_manual();
    return 0;
  }
  bool r;

  Options opt("FullTextSearch", "Okay, fts!");
  opt.add_options()("config", "XML user settings", value<std::string>()->default_value("config.xml"));

  ParseResult pr = opt.parse(argc, argv);

  if (pr.count("config")) {
    config_properties user_config (pr["config"].as<std::string>(), "user/");
    r = user_config.read_properties();

    ASSERT(r, "Не удалось найти конфиг");

    user_config.print_properties();

  }

  return 0;
}

void print_manual() {
  printf(
      "Для измерения результата сумматора необходимо ввести: \n"
      "\t- номер аргумента в виде слова (--first, --second) \n"
      "\t- вещественное число \n");
}
