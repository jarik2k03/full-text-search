#define FAIL -1
#define SUCCESS 0
#define ERRMSG(err, msg) \
  if (err == FAIL)       \
    std::cout << "Возникла ошибка: " << msg << '\n';
