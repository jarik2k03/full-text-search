#include "indexer.h"

IndexBuilder::IndexBuilder() : p({"to", "is", "with"}, 3, 6) {
}

IndexBuilder::IndexBuilder(cstr& books_name, cstr& config_name, cstr& path)
    : p(config_name, path) {
  std::ifstream books(path + books_name);
  ASSERT(!books, "Не найдена база книг для индексации!");
  str line;
  std::cout << path + books_name << "\n";

  cstr fullpath = path + "indexed/";
  cstr command = "mkdir -p " + fullpath;
  cstr docpath = fullpath + "/docs/";
  std::system((command + docpath).c_str());

  while (std::getline(books, line)) {
    const auto delim = line.find_first_of(',');
    cstr s_id(line, 0, delim);
    const int id = std::stoi(s_id);
    line.erase(0, delim + 1);
    add_document(id, line);
    fill_docs(docpath, s_id, line);
  }
  books.close();
}

void IndexBuilder::fill_docs(cstr& docpath, cstr& id, cstr& line) const {
  std::ofstream doc(docpath + id);
  doc << line;
  doc.close();
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

bool IndexBuilder::create_index(cstr& path) {
  cstr fullpath = p.get_binded_path() + path;
  str command = "mkdir -p " + fullpath;
  cstr entrypath = fullpath + "/entries/";
  std::system((command + entrypath).c_str());

  return false;
}

void TextIndexWriter::write(cstr& path) {
  // std::cout << path << "\n";
}