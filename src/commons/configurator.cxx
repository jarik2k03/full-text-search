#include "configurator.h"

Configurator::Configurator(cstr& path_to_config) {
  ASSERT(
      !userconfig.load_file(path_to_config.c_str()),
      "Не найдена конфигурация fts");
}
