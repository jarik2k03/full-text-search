#include "searcher.h"

void IndexProcessor::search(cstr& request, const IndexAccessor* access) {
}

TextIndexAccessor::TextIndexAccessor(
    const ParserOpts& po,
    const IdxWriterOpts& iwo,
    cstr& ip)
    : p_opts(po), w_opts(iwo), index_path(ip) {
}

forwardmap TextIndexAccessor::access_all_forward(int id) const noexcept {
  forwardmap docs;
  return docs;
}
InvertedIndex TextIndexAccessor::access_inverted(cstr& ngram) const noexcept {
  cstr part_ngram(ngram, 0, w_opts.part_length);
  cstr hash_name = name_to_hash(part_ngram);
  std::ifstream found_fileset(index_path + hash_name);
  ASSERT(found_fileset.bad(), "Не найден файл с нужными нграммами");

  str line;
  while (std::getline(found_fileset, line)) {
    cstr found_ngram = line.substr(0, ngram.size());
    
  }

  InvertedIndex entry;
  return entry;
}

str TextIndexAccessor::name_to_hash(cstr& name) const noexcept {
  return picosha2::hash256_hex_string(name).substr(0, w_opts.hash_length);
}