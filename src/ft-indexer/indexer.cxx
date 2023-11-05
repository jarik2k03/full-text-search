#include "indexer.h"

IndexBuilder::IndexBuilder(const booktagsvector& bt, int p_l, int h_l)
    : part_length(p_l), hash_length(h_l), p({"to", "is", "with"}, 3, 6) {
}

IndexBuilder::IndexBuilder(docmap& ld, booktagsvector& bt, int p_l, int h_l)
    : loaded_document(ld),
      book_tags(bt),
      part_length(p_l),
      hash_length(h_l),
      p({"to", "is", "with"}, 3, 6) {
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
    add_one_common(line);
  }
  books.close();
}
bool IndexBuilder::add_one_common(str& line) {
  auto it = book_tags.begin();
  auto delim = line.find_first_of(',');
  str bufstr = {0};
  bufstr.clear();
  CommonIndex pd;
  std::stringstream ss(line);

  std::getline(ss, bufstr, ',');
  cstr id = bufstr;
  while (std::getline(ss, bufstr, ',')) {
    ++it;
    if (it->second != -1) {
      pd.emplace_back(bufstr);
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
    for (const auto tag : pd) {
      std::cout << tag << '\t';
    }
  }
}

InvertedResult IndexBuilder::build_inverted() {
  InvertedResult ir;
  indexmap imap;
  // пре-инициализация индексных деревьев для каждого атрибута
  for (const auto& [tag, p] : book_tags) {
    if (p != -1)
      ir.full_index.push_back(imap);
  }
  // for: обход по докам (cols) - O (n)
  for (const auto& [id, doc] : loaded_document) {
    // for: обход по аттрибутам (rows) - O (m)
    for (int row_attr = 0; row_attr < ir.full_index.size() - 1; ++row_attr) {
      str unparsed_attr = doc.at(row_attr);
      ParserResult doc_ngrams = p.parse(unparsed_attr);
      // for: обход всех нграмм из атрибута и заполнение - O (nwords*(max-min))
      for (const auto& ngram_node : doc_ngrams.ngrams) {
        add_one_inverted(
            ir.full_index.at(row_attr), ngram_node, row_attr, std::stoi(id));
      }
    }
  }
  return ir;
}
void IndexBuilder::add_one_inverted(
    indexmap& imap,
    const std::pair<cstr, uint8_t>& ng,
    const int row,
    const int cur_id) const {
  auto node = imap.find(ng.first);
  if (node == imap.end()) {
    InvertedIndex new_node;
    node = imap.insert(node, {ng.first, new_node});
  }
  InvertedIndex& inv_index = node->second;
  auto inv_index__node = inv_index.map.find(cur_id);
  if (inv_index__node == inv_index.map.end()) {
    // добавить вхождение документа
    ++inv_index.doc_count;
    InvertedIndex_ new_inv_index_(ng.second);
    inv_index.map.insert({cur_id, new_inv_index_});
  } else {
    // при наличии документа добавить токен (во избежание дублирования)
    InvertedIndex_& inv_index_ = inv_index__node->second;
    ++inv_index_.pos_count;
    inv_index_.ntoken.emplace_back(ng.second);
  }
}

bool IndexBuilder::check_eq_tags(cstr& line, short pos) const {
  const auto& it = book_tags.begin() + pos - 1;
  if (it->first != line && it->second != -1)
    return true;
  return false;
}

TextIndexWriter::TextIndexWriter(
    docmap& dm,
    InvertedResult& ir,
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
void TextIndexWriter::write_common(cstr& path) const {
  ASSERT(docindex.empty(), "БД книг не найдена. Невозможна запись!");
  std::ofstream doc;
  cstr docpath = path + "/docs/";
  create_folder(docpath);
  for (const auto& [id, data] : (1, docindex)) {
    cstr namefile = docpath + id;
    doc.open(namefile + ".page", std::ios::out);
    for (const auto attr : data) {
      doc << attr << '\n';
    }
    doc.close();
  }
}

void TextIndexWriter::write_one_common(
    const CommonIndex& data,
    std::ofstream& file) const {
  for (const auto attr : data) {
    file << attr << '\n';
  }
}

void TextIndexWriter::write_inverted(cstr& path) const {
  ASSERT(
      indexresult.full_index.empty(), "БД книг не найдена. Невозможна запись!");
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
      write_one_inverted(idx, entr, ngram_fileset);
      prev_part = part;
    }
  }
}
void TextIndexWriter::write_one_inverted(
    cstr& ngram,
    const InvertedIndex& cur,
    std::ofstream& file) const {
  std::stringstream buffer;

  buffer << ngram << " " << cur.doc_count << " ";
  for (const auto& [id, idx] : cur.map) {
    buffer << id << " " << idx.pos_count << " ";
    for (auto i : idx.ntoken)
      buffer << +i << " ";
  }
  buffer << '\n';
  file << buffer.rdbuf();
}

void create_folder(cstr& fullpath) {
#ifdef __linux
  str command = "mkdir -p " + fullpath;
  int t = std::system(command.c_str());
#endif
}
