#include "indexer.h"

IndexBuilder::IndexBuilder() : p({"to", "is", "with"}, 3, 6) {
}

IndexBuilder::IndexBuilder(cstr& books_name, cstr& config_name)
    : p(config_name) {
  std::ifstream books(books_name);
  ASSERT(!books, "Не найдена база книг для индексации!");
  ASSERT(read_index_properties(config_name), "Не найден конфиг!");

  str line;
  std::cout << books_name << "\n";
  int count = 0;
  std::getline(books, line);
  std::stringstream ss(line);
  while (std::getline(ss, line, ',')) {
    ASSERT(check_eq_tags(line, ++count), "Несоответствие атрибута БД: " + line);
  }
  while (std::getline(books, line)) {
    add_document(line);
  }
  books.close();
}

void IndexBuilder::print_results() const {
  p.print_config();
  // for (const auto& [pr, id] : save_parsed) {
  //   std::cout << "Document id: " << id << "\n";
  //   pr.ngrams_traverse();
  // }
}

bool IndexBuilder::add_document(str& line) {
  auto it = book_tags.begin();
  auto delim = line.find_first_of(',');
  str bufstr = {0};
  bufstr.clear();
  ParsedDocument pd;
  std::stringstream ss(line);

  std::getline(ss, bufstr, ',');
  cstr id = bufstr;
  while (std::getline(ss, bufstr, ',')) {
    ++it;
    if (it->second != -1) {
      pd.tags.emplace_back(bufstr);
      pd.parsed.emplace_back(p.parse(bufstr));
    }
  }
  loaded_document.insert({id, pd});

  return false;
}

bool IndexBuilder::read_index_properties(cstr& config_name) {
  pugi::xml_document doc;
  const pugi::xml_parse_result parsed = doc.load_file(config_name.c_str());
  if (!parsed)
    return true;
  const pugi::xml_node indexer = doc.child("fts").child("indexer");
  part_length = indexer.attribute("part_length").as_int();
  hash_length = indexer.attribute("hash_length").as_int();
  int count = 0;
  for (pugi::xml_node i : indexer.children("field")) {
    auto ignore = i.attribute("ignore");
    ++count;
    cstr val = ignore.value();
    short c = (val == "true") ? -1 : count;
    book_tags.emplace_back(i.text().as_string(), c);
  }

  return false;
}

void IndexBuilder::print_index_properties() const {
  for (const auto& [key, pos] : book_tags)
    std::cout << "Book tag: " << key << "; Position: " << pos << '\n';
}

void IndexBuilder::print_documents() const {
  for (const auto& [id, pd] : loaded_document) {
    std::cout << "\n[" << id << "] ~ ";
    for (const auto tag : pd.tags) {
      std::cout << tag << '\t';
    }
  }
}

IndexerResult IndexBuilder::build_inverted_index() {
  IndexerResult ir;
  indexmap imap;
  for (const auto& [tag, p] : book_tags) {
    if (p != -1)
      ir.full_index.push_back(imap);
  }

  for (const auto& [id, doc] : loaded_document) {
    // std::cout << "--------ROW------\n";
    for (int pos = 0; pos < ir.full_index.size() - 1; ++pos) {
      for (const auto ngram : doc.parsed.at(pos).ngrams) {
        add_for_ngram(ir.full_index.at(pos), ngram.first, pos);
        // ir.traverse();
      }
    }
  }
  return ir;
}

void IndexBuilder::add_for_ngram(indexmap& imap, cstr& ngram, const int row)
    const {
  InvertedIndex indexed;

  for (const auto& [id, data] : loaded_document) {
    auto& ng = data.parsed.at(row).ngrams;
    InvertedIndex_ entr;
    auto node = ng.find(ngram);
    if (node != ng.end() && imap.find(ngram) == imap.end()) {
      auto range = ng.equal_range(ngram);
      for (auto d = range.first; d != range.second; ++d) { // ~ O(1)
        entr.ntoken.emplace_back(d->second);
        ++entr.pos_count;
      }
      entr.doc_id = id;
      indexed.entries.emplace_back(entr);
      ++indexed.doc_count;
    }
  }
  imap.insert({ngram, indexed});
}

bool IndexBuilder::check_eq_tags(cstr& line, short pos) const {
  const auto& it = book_tags.begin() + pos - 1;
  std::cout << "Line: " << line << " Pos: " << pos << '\n';
  if (it->first != line && it->second != -1)
    return true;
  return false;
}

TextIndexWriter::TextIndexWriter(
    docmap& dm,
    IndexerResult& ir,
    std::vector<std::pair<str, short>>& bm,
    const int p_l,
    const int h_l)
    : docindex(dm),
      indexresult(ir),
      book_tags(bm),
      part_length(p_l),
      hash_length(h_l) {
}

str TextIndexWriter::name_to_hash(cstr& name) const {
  return picosha2::hash256_hex_string(name).substr(0, hash_length);
}

void TextIndexWriter::write(cstr& path) const {
  ASSERT(docindex.empty(), "БД книг не найдена. Невозможна запись!");
  std::ofstream doc;
  cstr docpath = path + "/docs/";
  create_folder(docpath);
  for (const auto& [id, data] : (1, docindex)) {
    cstr namefile = docpath + id;
    doc.open(namefile + ".page", std::ios::out);
    for (const auto attr : data.tags) {
      doc << attr << '\n';
    }
    doc.close();
  }
  cstr entrpath = path + "/entries/";

  create_folder(entrpath);
  std::ofstream ngram_fileset;
  str prev_part = "";
  int attrpos = 0;
  for (auto it = book_tags.begin() + 1; it != book_tags.end(); ++it) {
    if (it->second == -1)
      continue;
    std::ostringstream attr_ngrams_path(entrpath);
    attr_ngrams_path << entrpath << it->first << "/";
    create_folder(attr_ngrams_path.str());
    for (const auto& [idx, entr] : indexresult.full_index.at(attrpos++)) {
      cstr part = name_to_hash(cstr(idx, 0, part_length));
      if (part != prev_part) {
        ngram_fileset.close();
        ngram_fileset.open(attr_ngrams_path.str() + part, std::ios::out);
      }
      write_one(idx, entr, ngram_fileset);
      prev_part = part;
    }
  }
}

void TextIndexWriter::write_one(
    cstr& ngram,
    const InvertedIndex& cur,
    std::ofstream& file) const {
  file << ngram << " " << cur.doc_count << " ";
  for (auto& idx : cur.entries) {
    file << idx.doc_id << " " << idx.pos_count << " ";
    for (auto i : idx.ntoken)
      file << i << " ";
  }
  file << '\n';
}

void create_folder(cstr& fullpath) {
#ifdef __linux
  str command = "mkdir -p " + fullpath;
  int t = std::system(command.c_str());
#endif
}
