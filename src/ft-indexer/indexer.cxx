#include "indexer.h"

IndexBuilder::IndexBuilder() : p({"to", "is", "with"}, 3, 6) {
}

IndexBuilder::IndexBuilder(cstr& books_name, cstr& config_name, cstr& path)
    : p(config_name, path) {
  std::ifstream books(path + books_name);
  ASSERT(!books, "Не найдена база книг для индексации!");
  str line;
  std::cout << path + books_name << "\n";

  while (std::getline(books, line)) {
    const std::size_t delim = line.find_first_of(',');
    const int id = std::stoi(cstr(line, 0, delim));
    add_document(id, line.erase(0, delim + 1));
  }
  books.close();
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

void TextIndexWriter::fill_docs(cstr& books_name, cstr& path) const {
  std::ifstream books(path + books_name);
  ASSERT(!books, "Не найдена база книг для индексации!");
  cstr docpath = path + "indexed/docs/";
  create_folder(docpath);
  str line;
  while (std::getline(books, line)) {
    std::size_t delim = line.find_first_of(',');
    std::ofstream doc(docpath + cstr(line, 0, delim));
    doc << line;
    doc.close();
  }
  books.close();
}

void TextIndexWriter::fill_entries(ngrams_vec& parses, cstr& path) const {
  cstr entrypath = path + "indexed/entries/";
  create_folder(entrypath);
  for (const auto& [pr, id] : parses) {
    for (const auto& [key, pos] : pr.ngrams) {
      cstr sha256key = picosha2::hash256_hex_string(key);
      std::ofstream entryfile(entrypath + cstr(sha256key, 0, 6));
      entryfile << key;
      entryfile.close();
    }
  }
}

void create_folder(cstr& fullpath) {
  str command = "mkdir -p " + fullpath;
  std::system(command.c_str());
}