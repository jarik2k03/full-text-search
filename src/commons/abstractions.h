#define ASSERT(error, msg) \
  if (error == true) {      \
    std::cout << "Возникла ошибка: " << msg << '\n'; \
    exit(-1); }


#define PEXCHANGE(from, to) \
  to = std::exchange(from, nullptr);

#define EXCHANGE(from, to) \
  to = std::exchange(from, 0);


using uint = unsigned int;
using cstr = const std::string;