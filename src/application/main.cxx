#include <cxxopts.hpp>

#include <commons/abstractions.h>
#include <commons/configurator.h>
#include <ft-indexer/indexer.h>
#include <unistd.h>
#include <iostream>

using namespace cxxopts;

int print_manual();

int main(int argc, char** argv) {
  if (argc == 1)
    return print_manual();

  try {
    Options opt("FullTextSearch", "Okay, fts!");
    opt.add_options()(
        "config",
        "XML user settings",
        value<str>()->implicit_value("user/config.xml"))(
        "book",
        "Books database .csv name",
        value<str>()->implicit_value("user/book.csv"))(
        "index",
        "folder to save the indexes from database",
        value<str>()->implicit_value("user/indexed/"));

    const ParseResult pr = opt.parse(argc, argv);
    if (pr.count("config")) {
      Configurator user_config(pr["config"].as<str>());
      if (pr.count("book")) {
        str book_name = pr["book"].as<str>();
        cstr index_folder = pr["index"].as<str>();
        IndexBuilder b(user_config.get_document());
        const IndexerResult ir = b.build_all(book_name);
        if (pr.count("index")) {
          IndexWriter* writer = new TextIndexWriter(user_config.get_document());
          writer->write_all_forward(ir.first, index_folder);
          writer->write_all_inverted(ir.second, index_folder);
        }
      }
    }
  } catch (const option_exists_error& e) {
    std::cout << "Error parsing options: " << e.what() << std::endl;
    exit(1);
  }
  return 0;
}

int print_manual() {
  std::cout << "--config - пользовательские настройки (default:config.xml)\n"
            << "--book - проиндексировать базу книг\n";
  return 0;
}
