#pragma once
#include <commons/parser.h>

#include <picosha2.h>

#include <chrono>
#include <fstream>
#include <map>
#include <vector>

using ngrams_vec = std::vector<std::pair<ParserResult, int>>;

class IndexWriter {
 public:
  virtual void write(cstr& path) = 0;
  virtual void fill_docs(cstr& books_name) const = 0;
  virtual void fill_entries(ngrams_vec& parsed) const = 0;
};

struct ParsedDocument {
  std::vector<str> tags;
  std::vector<ParserResult> parsed;
};

class IndexBuilder {
 private:
  Parser p;
  std::map<str, ParsedDocument> loaded_document;
  ngrams_vec save_parsed;
  std::vector<std::pair<str, short>> book_tags;
  bool add_document(str& line);

 public:
  IndexBuilder();
  IndexBuilder(cstr& books_name, cstr& config_name);

  bool read_index_properties(cstr& name);
  void print_index_properties() const;
  void print_documents() const;
  bool check_eq_tags(cstr& line, short pos) const;
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
  void fill_docs(cstr& books_name) const override;
  void fill_entries(ngrams_vec& parsed) const override;
};

void create_folder(cstr& name);
bool is_comma(char c);