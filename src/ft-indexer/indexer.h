#pragma once
#include <commons/parser.h>

#include <picosha2.h>

#include <fstream>
#include <vector>

using ngrams_vec = std::vector<std::pair<ParserResult, int>>;

class IndexWriter {
 public:
  virtual void write(cstr& path) = 0;
  virtual void fill_docs(cstr& books_name, cstr& path = "user/") const = 0;
  virtual void fill_entries(ngrams_vec& parsed, cstr& path = "user/") const = 0;
};

class IndexBuilder {
 private:
  Parser p;
  ngrams_vec save_parsed;
  bool add_document(const int doc_id, str& text);

 public:
  IndexBuilder();
  IndexBuilder(cstr& books_name, cstr& config_name, cstr& path = "user/");

  ngrams_vec& get_save_parsed() {
    return save_parsed;
  }

  void print_results() const;
};

class TextIndexWriter : public IndexWriter {
 protected:
  int a, b;

 public:
  void write(cstr& path) override;
  void fill_docs(cstr& books_name, cstr& path = "user/") const override;
  void fill_entries(ngrams_vec& parsed, cstr& path = "user/") const override;
};

void create_folder(cstr& name);