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
    usleep(1000);
    std::cout << "\n[" << id << "] ~ ";
    for (const auto tag : pd.tags) {
      std::cout << tag << '\t';
    }
  }
}

indexmap IndexBuilder::build_inverted_index() {
  indexmap map;

  InvertedIndex index;
  InvertedIndex_entries arr_tokens;
  auto doc = loaded_document.find("1");
  ParserResult pr = doc->second.parsed.back();
  // pr.ngrams_traverse();

  // for (const auto& [ngram, pos] : pr.ngrams) {
  //
  // }
  auto prince = pr.ngrams.begin();
  int counter = 0;
  for (const auto& [id, data] : loaded_document) {
    ParserResult pr1 = data.parsed.back();
    auto catched = pr1.ngrams.find(prince->first);
    if (catched != pr1.ngrams.end()) {
      std::cout << "CATCHED: " << catched->first
                << " pos: " << (short)catched->second << " in docID: " << id
                << '\n';
    }
    ++counter;
  }

  return map;
}

bool IndexBuilder::check_eq_tags(cstr& line, short pos) const {
  const auto& it = book_tags.begin() + pos - 1;
  std::cout << "Line: " << line << " Pos: " << pos << '\n';
  if (it->first != line && it->second != -1)
    return true;
  return false;
}

void TextIndexWriter::write(
    const docmap& csv,
    cstr& path,
    const int part_len,
    const int hash_len) const {
  ASSERT(csv.empty(), "БД книг не найдена. Невозможная запись!");
  std::ofstream doc;
  cstr docpath = path + "/docs/";
  create_folder(docpath);
  for (const auto& [id, data] : csv) {
    cstr namefile = docpath + id;
    doc.open(namefile + ".page", std::ios::out);
    for (const auto attr : data.tags) {
      doc << attr << '\n';
    }
    doc.close();
  }

  // cstr entrypath = path + "/entries/";
  // create_folder(entrypath);
  // for (const auto& [id, data] : csv) {
  //   cstr namefile = " ";
  //   doc.open(namefile, std::ios::out);
  //   for (const auto attr : data.tags) {
  //     doc << attr << '\n';
  //   }
  //   doc.close();
  // }
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

// void TextIndexWriter::fill_entries(cstr& parses) const {
//   cstr entrypath = "user/indexed/entries/";
//   create_folder(entrypath);
//   auto t1 = std::chrono::system_clock::now();

//   std::ofstream entryfile(entrypath);
//   for (const auto& [pr, id] : parses) {
//     entryfile.close();
//     cstr sha256key = picosha2::hash256_hex_string(std::to_string(id));
//     entryfile.open(entrypath + cstr(sha256key, 0, 6));
//     // std::cout << "Indexed " << counter << " documents.\n";
//     for (const auto& [key, pos] : pr.ngrams) {
//       entryfile << key << '\n';
//     }
//   }
//   auto t2 = std::chrono::system_clock::now();
//   std::chrono::duration<double> time = t2 - t1;
//   std::cout << "-- " << time.count() << "s. \n";
// }

void create_folder(cstr& fullpath) {
#ifdef __linux
  str command = "mkdir -p " + fullpath;
  int t = std::system(command.c_str());
#endif
}
