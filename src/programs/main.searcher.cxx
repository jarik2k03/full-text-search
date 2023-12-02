#include <cxxopts.hpp>
#include <replxx.hxx>

#include <commons/_abstractions.h>
#include <commons/configurator.h>
#include <ft-searcher/searcher.h>
#include <unistd.h>
#include <iostream>

int print_manual();

bool process_command(
    cstr& cmd,
    IndexProcessor& searcher,
    TextIndexAccessor& accessor,
    replxx::Replxx& t) {
  std::stringstream ss(cmd);
  str line;
  ss >> line;
  if (line == "+") {
    if (ss >> line) {
      searcher.add_attribute(line);
      std::cout << "Added search attribute: " << line << '\n';
    }
  } else if (line == "-") {
    if (ss >> line) {
      if (searcher.remove_attribute(line))
        std::cout << "Removed search attribute: " << line << '\n';
    }
  } else if (line == ">") {
    for (auto& [attribute, request] : searcher.get_attributes()) {
      cstr fts_label = "(fts::" + attribute;
      request = t.input(fts_label + ") ");
      t.history_add(request);
    }
    str title_request = ss.rdbuf()->str();
    const scoredocs documents = searcher.search(accessor, title_request);
    searcher.print_results(documents, 0, 24);
  } else if (line.empty()) {
    return false;
  }
  return true;
}

void search_with_xml_template(
    Configurator& config,
    cstr& indexpath,
    cstr& requestpath) {
  SearchState attrs = config.get_request_from_doc(requestpath);
  IndexProcessor searcher(config.get_parser_opts(), attrs);
  TextIndexAccessor accessor(config, indexpath);

  const scoredocs documents = searcher.search(accessor, attrs.title_request);
  searcher.print_results(documents, 0, 24);
}

void launch_interactive_mode(Configurator& config, cstr& indexpath) {
  IndexProcessor searcher(config.get_parser_opts());
  TextIndexAccessor accessor(config, indexpath);
  replxx::Replxx terminal;

  terminal.disable_bracketed_paste();
  terminal.history_load("user/.request_history");
  while (true) {
    cstr command = terminal.input("(fts) ");
    terminal.history_add(command);
    if (!process_command(command, searcher, accessor, terminal))
      break;
    std::cout << std::endl;
  }
  terminal.history_save("user/.request_history");
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
      search_with_xml_template(
          user_config, pr["index"].as<str>(), pr["query"].as<str>());
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
