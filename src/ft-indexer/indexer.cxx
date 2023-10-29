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

  // books.close();
}

void IndexBuilder::print_results() const {
  p.print_config();
  for (const auto& [pr, id] : save_parsed) {
    std::cout << "Document id: " << id << "\n";
    pr.ngrams_traverse();
  }
}

bool IndexBuilder::add_document(str& line) {
  auto it = book_tags.begin();
  auto delim = line.find_first_of(',');
  str bufstr("");
  ParsedDocument pd;
  it++;
  for (size_t i = delim; it != book_tags.end(); ++i) {
    if (line.find(i) == ',') {
      if (!bufstr.empty()) {
        bufstr.push_back('\0');
        pd.tags.emplace_back(bufstr);
        pd.parsed.emplace_back(p.parse(bufstr));
        bufstr.clear();
      }
      ++it;
    }
    if (it->second != -1) {
      bufstr.push_back(line.find(i));
    }
  }
  cstr id(line, 0, delim);
  loaded_document.insert({id, pd});

  return false;
}

bool IndexBuilder::read_index_properties(cstr& name) {
  pugi::xml_document doc;
  const pugi::xml_parse_result parsed = doc.load_file(name.c_str());
  if (!parsed)
    return true;
  const pugi::xml_node indexer = doc.child("fts").child("indexer");
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
    std::cout << "\n~ ";
    for (const auto tag : pd.tags) {
      std::cout << tag << '\t';
    }
  }
}

bool IndexBuilder::check_eq_tags(cstr& line, short pos) const {
  const auto& it = book_tags.begin() + pos - 1;
  std::cout << "Line: " << line << " Pos: " << pos << '\n';
  if (it->first != line && it->second != -1)
    return true;
  return false;
}

void TextIndexWriter::write(cstr& path) {
  // std::cout << path << "\n";
}

void TextIndexWriter::fill_docs(cstr& books_name) const {
  std::ifstream books(books_name);
  ASSERT(!books, "Не найдена база книг для индексации!");
  cstr docpath = "user/indexed/docs/";
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

void TextIndexWriter::fill_entries(ngrams_vec& parses) const {
  cstr entrypath = "user/indexed/entries/";
  create_folder(entrypath);
  auto t1 = std::chrono::system_clock::now();

  std::ofstream entryfile(entrypath);
  for (const auto& [pr, id] : parses) {
    entryfile.close();
    cstr sha256key = picosha2::hash256_hex_string(std::to_string(id));
    entryfile.open(entrypath + cstr(sha256key, 0, 6));
    // std::cout << "Indexed " << counter << " documents.\n";
    for (const auto& [key, pos] : pr.ngrams) {
      entryfile << key << '\n';
    }
  }
  auto t2 = std::chrono::system_clock::now();
  std::chrono::duration<double> time = t2 - t1;
  std::cout << "-- " << time.count() << "s. \n";
}

void create_folder(cstr& fullpath) {
#ifdef __linux
  str command = "mkdir -p " + fullpath;
  std::system(command.c_str());
#endif
}

bool is_comma(char c) {
  return (c == ',') ? true : false;
}