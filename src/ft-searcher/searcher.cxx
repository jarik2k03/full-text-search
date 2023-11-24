#include "searcher.h"

void IndexProcessor::search(TextIndexAccessor& access) {
  access_all_docs_dat(access.get_index_path());
  // поиск обратного индекса для всех атрибутов
  for (auto& [attr, request] : search_attrs) {
    const ParserResult pr = parser.parse(request);
    invertedmap entries = access.access_inverted(pr, attr);
    scoremap docs = access.access_forward(entries);
    calc_score(pr, docs, entries);
    print_results(docs);
  }
}

void IndexProcessor::print_results(scoremap& map) const noexcept {
  uint counter = 0;
  std::cout << "N\tRating\tBookID\tTitle\n";
  for (const auto& [id, title] : map) {
    if (title.second > 0)
      std::cout << ++counter << '\t' << title.second << '\t' << id << '\t'
                << title.first.at(0) << '\n';
  }
}

void IndexProcessor::calc_score(
    const ParserResult& pr,
    scoremap& docs,
    const invertedmap& entries) {
  double score = 0.0;
  for (const auto& [id, title] : docs) {
    for (const auto& [ngram, p] : pr.ngrams) {
      InvertedIndex ii = entries.find(ngram)->second;
      const double dfreq = df(ngram, ii);
      if (dfreq == 0)
        continue;
      const double tfreq = tf(ngram, id, ii);
      // const double idf = log();
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

scoremap TextIndexAccessor::access_forward(invertedmap& im) const noexcept {
  scoremap docs;

  std::ifstream found_document;
  for (const auto& [id, ii] : im) {
    for (const auto& [id, entries] : ii.map) {
      cstr s_id = std::to_string(id);
      cstr docpath = index_path + "docs/";
      cstr index_docpath = docpath + s_id;
      found_document.open(index_docpath + ".page");
      ASSERT(found_document.bad(), "Не найден документ");
      docs.insert({id, make_pair(read_forward(found_document), 0.0)});
      found_document.close();
    }
  }

  return docs;
}
invertedmap TextIndexAccessor::access_inverted(
    const ParserResult& ngrams,
    cstr& attr_name) const noexcept {
  invertedmap imap;
  for (const auto& ngram : ngrams.ngrams) {
    cstr part_ngram(ngram.first, 0, w_opts.part_length);
    cstr hash_name = name_to_hash(part_ngram);
    // std::cout << hash_name << '\n';
    cstr entrpath = index_path + "entries/";
    cstr entr_attr_path = entrpath + attr_name + "/";
    std::ifstream found_fileset(entr_attr_path + hash_name);
    if (!found_fileset.is_open())
      return imap;
    // std::cout << entr_attr_path + hash_name << "\n";

    str line;
    while (std::getline(found_fileset, line)) {
      cstr found_ngram = line.substr(0, line.find_first_of(' '));
      if (found_ngram == ngram.first) {
        imap.insert({found_ngram, read_inverted(line)});
        break;
      }
    }
  }
  return imap;
}

forwardIndex TextIndexAccessor::read_forward(
    std::ifstream& fin) const noexcept {
  forwardIndex doc;
  str line;
  while (std::getline(fin, line)) {
    doc.emplace_back(line);
  }
  return doc;
}

InvertedIndex TextIndexAccessor::read_inverted(cstr& line) const noexcept {
  InvertedIndex entry;
  str fragment;
  std::stringstream ss(line);
  ss >> fragment;
  cstr ngram = fragment;
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
  return entry;
}

bool IndexProcessor::access_all_docs_dat(cstr& idx_path) noexcept {
  std::ifstream all_docs_dat(idx_path + "all_docs.dat");
  if (!all_docs_dat)
    return true;
  all_docs_dat >> all_docs;
  return false;
}

uint tf(cstr& term, int document_id, InvertedIndex& cur) {
  auto entry = cur.map.find(document_id);
  if (entry == cur.map.end())
    return 0;
  return entry->second.pos_count;
}

uint df(cstr& term, InvertedIndex& cur) {
  return cur.doc_count;
}
str TextIndexAccessor::name_to_hash(cstr& name) const noexcept {
  return picosha2::hash256_hex_string(name).substr(0, w_opts.hash_length);
}