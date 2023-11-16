#include <cxxopts.hpp>
#include <replxx.hxx>

#include <commons/_abstractions.h>
#include <commons/configurator.h>
#include <ft-searcher/searcher.h>
#include <unistd.h>
#include <iostream>

int print_manual();

void launch_interactive_mode(Configurator& config, cstr& indexpath) {
  replxx::Replxx terminal;
  IndexProcessor processor;
  IndexAccessor* accessor = new TextIndexAccessor(
      config.get_parser_opts(), config.get_writer_opts(), indexpath);
  terminal.disable_bracketed_paste();
  terminal.history_load("user/.request_history");
  while (true) {
    cstr request = terminal.input("(fts) ");
    if (request.empty())
      break;
    terminal.history_add(request);
    processor.search(request, accessor);
  }
  terminal.history_save("user/.request_history");
  delete accessor;
}
int main(int argc, char** argv) {
  if (argc == 1)
    return print_manual();

  try {
    cxxopts::Options opt("FullTextSearch", "Okay, fts!");
    opt.add_options()(
        "query",
        "User query",
        cxxopts::value<str>()->implicit_value("Steven King"))(
        "config",
        "XML user config",
        cxxopts::value<str>()->implicit_value("user/config.xml"))(
        "index",
        "folder to search need index",
        cxxopts::value<str>()->implicit_value("user/indexed/"));

    const cxxopts::ParseResult pr = opt.parse(argc, argv);

    if (!pr.count("config")) {
      std::cerr << "Для работы требуется конфиг\n";
      exit(-2);
    }
    Configurator user_config(pr["config"].as<str>());
    if (!pr.count("query")) {
      launch_interactive_mode(user_config, pr["index"].as<str>());
    } else {
      std::cout << "Команда разработчиков приносит извинения. Сырое фуфло не "
                   "имеет поддержки запросов.\n";
      exit(-2);
    }

  } catch (const cxxopts::option_exists_error& e) {
    std::cout << "Error parsing options: " << e.what() << std::endl;
    exit(1);
  }
  return 0;
}

int print_manual() {
  std::cout << "--query - пользовательский запрос (default:query.xml)\n"
            << "--index - папка с индексами для поиска\n";
  return 0;
}
