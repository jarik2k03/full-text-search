#include "indexer.h"

IndexBuilder::IndexBuilder() : p({"to", "is", "with"}, 3, 6) {
}

IndexBuilder::IndexBuilder(cstr& config_name, cstr& path)
    : p(config_name, path) {
}

void IndexBuilder::print_results() const {
  p.print_config();

  for (const auto& [pr, id] : save_parsed) {
    std::cout << "Document id: " << id << "\n";
    pr.ngrams_traverse();
  }
}

bool IndexBuilder::add_document(int doc_id, str text) {
  const ParserResult pr = p.parse(text);
  save_parsed.emplace_back(pr, doc_id);

  return false;
}

void TextIndexWriter::write(cstr& path) {
  // std::cout << path << "\n";
}