#include "fsp.h"


fsp_calculator::fsp_calculator()
{
    result = 0;
}

fsp_calculator::~fsp_calculator()
{
}

int fsp_calculator::summ(const char** argv)
{
    double _st, _nd;
    if (!strcmp(argv[1], "--first")) {
        _st = atof(argv[2]);
    } else return -1;
    if (!strcmp(argv[3], "--second")) {
        _nd = atof(argv[4]);
    } else return -1;
    
    result = _st + _nd;

    return 0;
}

void fsp_calculator::print_result()
{
    std::cout << "Хранимый результат: " << result << "\n";
}