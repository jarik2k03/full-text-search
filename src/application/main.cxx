#include <cxxopts.hpp>

#include <iostream>

#include <commons/abstractions.h>
#include <ft-indexer/indexer.h>

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
      if (pr.count("book")) {
        str book_name = pr["book"].as<str>();
        cstr config_name = pr["config"].as<str>();
        cstr index_folder = pr["index"].as<str>();

        IndexBuilder b(book_name, config_name);
        b.build_inverted_index();
        // b.print_index_properties();
        // b.print_documents();
        if (pr.count("index")) {
          IndexWriter* writer = new TextIndexWriter();
          // writer->write(
          //     b.loaded_document,
          //     index_folder,
          //     b.get_part_length(),
          //     b.get_hash_length());
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
