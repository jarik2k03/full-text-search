#include <cxxopts.hpp>

#include <commons/abstractions.h>
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
      if (pr.count("book")) {
        str book_name = pr["book"].as<str>();
        cstr config_name = pr["config"].as<str>();
        cstr index_folder = pr["index"].as<str>();

        IndexBuilder b(book_name, config_name);
        auto t1 = clock();
        IndexerResult ir = b.build_inverted_index();
        auto t2 = clock();
        std::cout << "Time elapsed: " << (double)(t2 - t1) / 1e+6 << '\n';
        if (pr.count("index")) {
          IndexWriter* writer = new TextIndexWriter(
              b.loaded_document,
              ir,
              b.book_tags,
              b.get_part_length(),
              b.get_hash_length());
          t1 = clock();
          writer->write(index_folder);
          t2 = clock();
          std::cout << "Write elapsed: " << (double)(t2 - t1) / 1e+6 << '\n';
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
