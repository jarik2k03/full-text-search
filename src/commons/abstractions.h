#ifndef ABSTRACTIONS_H
#define ABSTRACTIONS_H

#pragma once
#include <iostream>

#define ASSERT(error, msg)                              \
  if (error == true) {                                  \
    std::cerr << "Критическая ошибка: " << msg << '\n'; \
    exit(-1);                                           \
  }

#define PEXCHANGE(from, to) to = std::exchange(from, nullptr);

#define EXCHANGE(from, to) to = std::exchange(from, 0);

using std::begin;
using std::end;
using uint = unsigned int;
using cint = const int;
using cstr = const std::string;
using str = std::string;
using std::make_pair;

#endif