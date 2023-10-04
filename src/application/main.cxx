#include <commons/parser.h>
#include <cxxopts.hpp>
#include <pugixml.hpp>
#include <iostream>
#include "macros.h"

using namespace cxxopts;

void print_manual();

int main(int argc, char** argv) {
  if (argc == 1) {
    print_manual();
    return 0;
  }

  Options opt("FullTextSearch", "Okay, fts!");
  opt.add_options()("first", "First num", value<double>())(
      "second", "Second num", value<double>());

  ParseResult pr = opt.parse(argc, argv);

  if (pr.count("first") == 1 && pr.count("second") == 1) {
    double n1 = pr["first"].as<double>();
    double n2 = pr["second"].as<double>();
    fsp_calculator calc;
    double sum = calc.summ(n1, n2);
    calc.print_value(sum);
  }

  return 0;
}

void print_manual() {
  printf(
      "Для измерения результата сумматора необходимо ввести: \n"
      "\t- номер аргумента в виде слова (--first, --second) \n"
      "\t- вещественное число \n");
}
