#include "indexer.h"

IndexBuilder::IndexBuilder() : p({"to", "is", "with", "and", "on"}, 3, 6) {
}

IndexBuilder::IndexBuilder(const booktagsvector& bt)
    : p({"to", "is", "with", "and", "on"}, 3, 6), book_tags(bt) {
}

IndexBuilder::IndexBuilder(const pugi::xml_document& d) : p(d) {
  ASSERT(set_data(d), "Не найден конфиг!");
  ;
}
IndexBuilder::IndexBuilder(const commonmap& ld, const booktagsvector& bt)
    : loaded_document(ld),
      book_tags(bt),
      p({"to", "is", "with", "and", "on"}, 3, 6) {
}

IndexBuilder::IndexBuilder(const pugi::xml_document& d, cstr& books_name)
    : p(d) {
  ASSERT(set_data(d), "Не найден конфиг!");
  loaded_document = build_common(books_name);
}

commonmap IndexBuilder::build_common(std::vector<str>& ram_book) {
  commonmap new_index;
  str line = ram_book.at(0);
  int count = 0;
  std::stringstream ss(line);
  while (std::getline(ss, line, ',')) {
    ASSERT(check_eq_tags(line, ++count), "Несоответствие атрибута БД: " + line);
  }
  for (auto l = ram_book.begin() + 1; l != ram_book.end(); ++l) {
    add_one_common(new_index, *l);
  }
  return new_index;
}
commonmap IndexBuilder::build_common(cstr& book_name) {
  std::ifstream book(book_name);
  ASSERT(!book, "Не найдена база книг для индексации!");
  commonmap new_index;
  str line;
  std::cout << book_name << "\n";
  int count = 0;
  std::getline(book, line);
  std::stringstream ss(line);
  while (std::getline(ss, line, ',')) {
    ASSERT(check_eq_tags(line, ++count), "Несоответствие атрибута БД: " + line);
  }
  while (std::getline(book, line)) {
    add_one_common(new_index, line);
  }
  book.close();
  return new_index;
}
bool IndexBuilder::add_one_common(commonmap& doc, str& line) {
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
  doc.insert({id, pd});
  return false;
}
bool IndexBuilder::set_data(const pugi::xml_document& d) {
  const pugi::xml_node indexer = d.child("fts").child("indexer");
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
  InvertedResult new_ir;
  invertedmap imap;
  ASSERT(
      loaded_document.empty(),
      "БД книг не создана! Используйте конструктор build_inverted с внешним "
      "прямым индексом (commonmap) или назначьте его в IndexBuilder.");
  // пре-инициализация индексных деревьев для каждого атрибута
  for (const auto& [tag, p] : book_tags) {
    if (p != -1)
      new_ir.full_index.push_back(imap);
  }
  // for: обход по докам (cols) - O (n)
  for (const auto& [id, doc] : loaded_document) {
    // for: обход по аттрибутам (rows) - O (m)
    for (int row_attr = 0; row_attr < new_ir.full_index.size() - 1;
         ++row_attr) {
      str unparsed_attr = doc.at(row_attr);
      ParserResult doc_ngrams = p.parse(unparsed_attr);
      // for: обход всех нграмм из атрибута и заполнение - O (nwords*(max-min))
      for (const auto& ngram_node : doc_ngrams.ngrams) {
        add_one_inverted(
            new_ir.full_index.at(row_attr),
            ngram_node,
            row_attr,
            std::stoi(id));
      }
    }
  }
  return new_ir;
}
InvertedResult IndexBuilder::build_inverted(const commonmap& external) {
  InvertedResult new_ir;
  invertedmap imap;
  // пре-инициализация индексных деревьев для каждого атрибута
  for (const auto& [tag, p] : book_tags) {
    if (p != -1)
      new_ir.full_index.push_back(imap);
  }
  // for: обход по докам (cols) - O (n)
  for (const auto& [id, doc] : external) {
    // for: обход по аттрибутам (rows) - O (m)
    for (int row_attr = 0; row_attr < new_ir.full_index.size() - 1;
         ++row_attr) {
      str unparsed_attr = doc.at(row_attr);
      ParserResult doc_ngrams = p.parse(unparsed_attr);
      // for: обход всех нграмм из атрибута и заполнение - O (nwords*(max-min))
      for (const auto& ngram_node : doc_ngrams.ngrams) {
        add_one_inverted(
            new_ir.full_index.at(row_attr),
            ngram_node,
            row_attr,
            std::atoi(id.c_str()));
      }
    }
  }
  return new_ir;
}

void IndexBuilder::add_one_inverted(
    invertedmap& imap,
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

TextIndexWriter::TextIndexWriter(const pugi::xml_document& d) {
  set_data(d);
}
TextIndexWriter::TextIndexWriter(const booktagsvector& bt, cint p_l, cint h_l)
    : book_tags(bt), part_length(p_l), hash_length(h_l) {
}

bool TextIndexWriter::set_data(const pugi::xml_document& d) {
  const pugi::xml_node indexer = d.child("fts").child("indexer");
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

str TextIndexWriter::name_to_hash(cstr& name) const {
  return picosha2::hash256_hex_string(name).substr(0, hash_length);
}
void TextIndexWriter::write_common(const commonmap& docindex, cstr& path)
    const {
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

void TextIndexWriter::write_inverted(
    const InvertedResult& indexresult,
    cstr& path) const {
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
