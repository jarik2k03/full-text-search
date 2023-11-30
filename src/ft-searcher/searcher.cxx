#include "searcher.h"

void IndexProcessor::search(TextIndexAccessor& access, str& title_request) {
  access_all_docs_dat(access.get_index_path());
  // поиск обратного индекса для всех атрибутов
  const ParserResult pr = parser.parse(title_request);
  invertedmap entries = access.access_inverted(pr, "Title");
  doc_list = access.access_forward(entries);
  calc_score(pr, entries);

  for (auto& [attr, request] : search_attrs) {
    const ParserResult pr = parser.parse(request);
    const invertedmap entr = access.access_inverted(pr, attr);
    scoremap docIDmap = access.access_id(entr);

    calc_score(pr, docIDmap, entr);
    summ_score(docIDmap);
  }
  sort_print_results(0);
}

void IndexProcessor::sort_print_results(cint row, cint ncols) const noexcept {
  scoredocs res(doc_list.begin(), doc_list.end());

  std::sort(
      res.begin(),
      res.end(),
      [](const std::pair<uint, scoredoc>& _1,
         const std::pair<uint, scoredoc>& _2) {
        return _1.second.score > _2.second.score;
      });

  uint counter = 0;
  std::cout << "N\tRating\tBookID\tTitle\n";
  for (const auto& [id, doc] : res) {
    if (counter < ncols)
      std::cout << ++counter << '\t' << doc.score << '\t' << id << '\t'
                << doc.document.at(row) << '\n';
  }
}

void IndexProcessor::summ_score(scoremap& doc_ids) {
  for (auto& [id, score] : doc_ids) {
    auto doc_node = doc_list.find(id);
    if (doc_node != doc_list.end()) {
      doc_node->second.score += score;
    }
  }
}

void IndexProcessor::calc_score(
    const ParserResult& pr,
    const invertedmap& entries) {
  for (auto& [id, docs] : doc_list) {
    for (const auto& [ngram, p] : pr.ngrams) {
      auto iter = entries.find(ngram);
      if (iter == entries.end())
        continue;
      InvertedIndex ii = iter->second;
      const double dfreq = df(ngram, ii);
      if (dfreq == 0)
        continue;
      const double tfreq = tf(ngram, id, ii);
      const double idf = log(static_cast<double>(all_docs) / dfreq);
      docs.score += tfreq * idf;
    }
  }
}

void IndexProcessor::calc_score(
    const ParserResult& pr,
    scoremap& docs,
    const invertedmap& entries) {
  for (auto& [id, docs] : docs) {
    for (const auto& [ngram, p] : pr.ngrams) {
      auto iter = entries.find(ngram);
      if (iter == entries.end())
        continue;
      InvertedIndex ii = iter->second;
      const double dfreq = df(ngram, ii);
      if (dfreq == 0)
        continue;
      const double tfreq = tf(ngram, id, ii);
      const double idf = log(static_cast<double>(all_docs) / dfreq);
      docs += tfreq * idf;
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
}

IndexProcessor::IndexProcessor(const ParserOpts& po, const SearchState& s)
    : parser(po), search_attrs(s._search_attrs) {
}

TextIndexAccessor::TextIndexAccessor(const Configurator& c, cstr& ip)
    : p_opts(c.get_parser_opts()),
      w_opts(c.get_writer_opts()),
      b_opts(c.get_builder_opts()),
      index_path(ip) {
}

scoreforwardmap TextIndexAccessor::access_forward(
    const invertedmap& im) const noexcept {
  scoreforwardmap docs;
  std::ifstream found_document;
  for (const auto& [id, ii] : im) {
    for (const auto& [id, entries] : ii.map) {
      cstr s_id = std::to_string(id);
      cstr docpath = index_path + "docs/";
      cstr index_docpath = docpath + s_id;
      found_document.open(index_docpath + ".page");
      ASSERT(found_document.bad(), "Не найден документ");
      scoredoc doc{read_forward(found_document), 0.0};
      docs.insert({static_cast<uint>(id), doc});
      found_document.close();
    }
  }

  return docs;
}

scoremap TextIndexAccessor::access_id(const invertedmap& im) const noexcept {
  scoremap docs;
  for (const auto& [id, ii] : im) {
    for (const auto& [id, entries] : ii.map) {
      docs.insert({static_cast<uint>(id), 0.0});
    }
  }

  return docs;
}

invertedmap TextIndexAccessor::access_inverted(
    const ParserResult& ngrams,
    cstr& attr_name) const noexcept {
  invertedmap imap;

  str prev_part_ngram;
  std::ifstream found_fileset;

  for (const auto& ngram : ngrams.ngrams) {
    cstr part_ngram(ngram.first, 0, w_opts.part_length);
    if (prev_part_ngram != part_ngram) {
      found_fileset.close();
      cstr hash_name = name_to_hash(part_ngram);
      cstr entrpath = index_path + "entries/";
      cstr entr_attr_path = entrpath + attr_name + "/";
      found_fileset.open(entr_attr_path + hash_name);
      if (!found_fileset.is_open()) {
        return imap;
      }
    }
    str line;
    found_fileset.seekg(0);
    while (std::getline(found_fileset, line)) {
      cstr found_ngram = line.substr(0, line.find_first_of(' '));
      if (found_ngram == ngram.first) {
        imap.insert({found_ngram, read_inverted(line)});
        break;
      }
    }
    prev_part_ngram = part_ngram;
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