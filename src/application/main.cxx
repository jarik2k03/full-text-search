#include <cxxopts.hpp>

#include <iostream>

#include <commons/abstractions.h>
#include <ft-indexer/indexer.h>

using namespace cxxopts;

int print_manual();

int main(int argc, char** argv) {
  if (argc == 1)
    return print_manual();

  Options opt("FullTextSearch", "Okay, fts!");
  opt.add_options()(
      "config",
      "XML user settings",
      value<str>()->implicit_value("user/config.xml"))(
      "book",
      "Books database .csv name",
      value<str>()->implicit_value("user/book.csv"));

  const ParseResult pr = opt.parse(argc, argv);
  if (pr.count("config")) {
    if (pr.count("book")) {
      str book_name = pr["book"].as<str>();
      cstr config_name = pr["config"].as<str>();
      IndexBuilder builder(book_name, config_name);
      builder.print_index_properties();
      builder.print_documents();
      IndexWriter* writer = new TextIndexWriter();
      // writer->fill_docs(book_name);
      writer->fill_entries(builder.get_save_parsed());
      // builder.print_results();
    }
  }
  return 0;
}

int print_manual() {
  std::cout << "--config - пользовательские настройки (default:config.xml)\n"
            << "--book - проиндексировать базу книг\n";
  return 0;
}
