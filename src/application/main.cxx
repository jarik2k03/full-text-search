#include <iostream>
#include <fsp/fsp.h>
#include <cxxopts.hpp>
#include "macros.h"

void print_manual();


int main(int argc, const char **argv)
{
    if (argc < 4) {
        print_manual();
        return 0;
    }
    fsp_calculator calc;

    ERRMSG(calc.summ(argv), "Неверные аргументы или переполнение");
    calc.print_result();


    return 0;

}

void print_manual() {
    printf("Для измерения результата сумматора необходимо ввести: \n"
    "\t- номер аргумента в виде слова (--first, --second) \n"
    "\t- вещественное число \n");
}
