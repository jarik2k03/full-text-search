#define ASSERT(error, msg) \
  if (error == true) {      \
    std::cout << "Возникла ошибка: " << msg << '\n'; \
    exit(-1); }
