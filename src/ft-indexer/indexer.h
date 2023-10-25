#pragma once
#include <commons/parser.h>

// #include <picosha.h>

#include <fstream>
#include <vector>

using ngrams_vec = std::vector<std::pair<ParserResult, int>>;

class IndexWriter {
 public:
  virtual void write(cstr& path) = 0;
};

class IndexBuilder {
 private:
  Parser p;
  ngrams_vec save_parsed;
  bool add_document(const int doc_id, str& text);
  void fill_docs(cstr& docpath, cstr& id, cstr& line) const;

 public:
  IndexBuilder();
  IndexBuilder(cstr& books_name, cstr& config_name, cstr& path = "user/");

  bool create_index(cstr& path = "indexed/");
  void print_results() const;
};

class TextIndexWriter : public IndexWriter {
 protected:
  int a, b;

 public:
  void write(cstr& path);
};
