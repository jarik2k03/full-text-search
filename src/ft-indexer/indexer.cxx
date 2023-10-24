#include "indexer.h"

IndexBuilder::IndexBuilder() : p({"to", "is", "with"}, 3, 6) {


}

IndexBuilder::IndexBuilder(cstr& books_name, cstr& config_name, cstr& path)
    : p(config_name, path) {
  books.open(path + books_name);
  ASSERT(!books, "Не найдена база книг для индексации!");
  str line;
  std::cout << path + books_name << "\n";
  while (std::getline(books, line)) {
    const auto delim = line.find_first_of(',');
    const int id = std::stoi(str(line, 0, delim));
    add_document(id, line.erase(0, delim + 1));
  }
}

void IndexBuilder::print_results() const {
  p.print_config();

  for (const auto& [pr, id] : save_parsed) {
    std::cout << "Document id: " << id << "\n";
    pr.ngrams_traverse();
  }
}

bool IndexBuilder::add_document(const int doc_id, str& text) {
  const ParserResult pr = p.parse(text);
  save_parsed.emplace_back(pr, doc_id);

  return false;
}

void TextIndexWriter::write(cstr& path) {
  // std::cout << path << "\n";
}