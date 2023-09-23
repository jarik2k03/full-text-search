#pragma once

#include <string.h>
#include <iostream>

class fsp_calculator {
 private:
  double result;

 public:
  fsp_calculator();
  ~fsp_calculator();
  double summ(double _st, double _nd);
  void print_value(double value);
};