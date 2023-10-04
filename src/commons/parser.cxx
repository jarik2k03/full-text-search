#include "parser.h"

fsp_calculator::fsp_calculator() {
  result = 0;
}

fsp_calculator::~fsp_calculator() {
}

double fsp_calculator::summ(double _st, double _nd) {
  return _st + _nd;
}

void fsp_calculator::print_value(double value) {
  std::cout << "Хранимый результат: " << value << "\n";
}