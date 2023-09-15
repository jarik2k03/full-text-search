#pragma once

#include <vector>
#include <string.h>
#include <iostream>

class fsp_calculator
{
private:
    double result;
public:
    fsp_calculator();
    ~fsp_calculator();
    int summ(const char** argv);
    void print_result();
};