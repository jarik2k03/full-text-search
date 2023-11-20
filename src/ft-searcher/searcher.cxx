#include "searcher.h"

void IndexProcessor::search(IndexAccessor* access) {
  searched res;
  // поиск обратного индекса для всех атрибутов
  for (auto& [attr, request] : search_attrs) {
    const ParserResult pr = parser.parse(request);
    for (const auto& i : pr.ngrams) {
      InvertedIndex entry = access->access_inverted(i.first, attr);
      forwardmap docs = access->access_all_forward(entry);

    }
  }
}

void IndexProcessor::add_attribute(cstr& attr) {
  search_attrs.insert({attr, str()});
}

int IndexProcessor::remove_attribute(cstr& attr) {
  return search_attrs.erase(attr);
}
std::map<str, str>& IndexProcessor::get_attributes() {
  return search_attrs;
}

IndexProcessor::IndexProcessor(const ParserOpts& po) : parser(po) {
  add_attribute("Title"); // атрибут по умолчанию
}

TextIndexAccessor::TextIndexAccessor(
    const ParserOpts& po,
    const IdxWriterOpts& iwo,
    const IdxBuilderOpts& ibo,
    cstr& ip)
    : p_opts(po), w_opts(iwo), b_opts(ibo), index_path(ip) {
}

forwardmap TextIndexAccessor::access_all_forward(InvertedIndex& ii) const noexcept {
  forwardmap docs;

  std::ifstream found_document;
  for (const auto& [id, entries] : ii.map) {
    cstr s_id = std::to_string(id);
    cstr docpath = index_path + "docs/";
    cstr index_docpath = docpath + s_id;
    found_document.open(index_docpath + ".page");
    ASSERT(found_document.bad(), "Не найден документ");
    // while (std::getline())
    // {
    //   /* code */
    // }
    

    
  }
  

  return docs;
}
InvertedIndex TextIndexAccessor::access_inverted(cstr& ngram, cstr& attr_name) const noexcept {
  InvertedIndex entry;
  cstr part_ngram(ngram, 0, w_opts.part_length);
  cstr hash_name = name_to_hash(part_ngram);
  std::cout << hash_name << '\n';
  cstr entrpath = index_path + "entries/";
  cstr entr_attr_path = entrpath + attr_name + "/";
  std::ifstream found_fileset(entr_attr_path + hash_name);
  if (!found_fileset.is_open())
    return entry;
  std::cout << entr_attr_path + hash_name << "\n";

  str line;
  while (std::getline(found_fileset, line)) {
    cstr found_ngram = line.substr(0, ngram.size());
    if (found_ngram == ngram)
      goto fill_entry;
  }

fill_entry:
  
  str fragment;
  std::stringstream ss(line);
  ss >> fragment;
  ss >> fragment;
  entry.doc_count = std::stoi(fragment);
  while (ss >> fragment) {
    InvertedIndex::Entries e;
    int doc_id = std::stoi(fragment);
    ss >> fragment;
    e.pos_count = std::stoi(fragment);
    for (int i = 0; i < e.pos_count; ++i) {
      ss >> fragment;
      e.ntoken.emplace_back(std::stoi(fragment));
    }
    entry.map.insert({doc_id, e});
  }
  std::cout << ngram << " ";
  entry.print_format();
  return entry;
}

str TextIndexAccessor::name_to_hash(cstr& name) const noexcept {
  return picosha2::hash256_hex_string(name).substr(0, w_opts.hash_length);
}