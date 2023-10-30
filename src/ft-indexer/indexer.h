#pragma once
#include <commons/parser.h>

#include <picosha2.h>

#include <unistd.h>
#include <chrono>
#include <fstream>
#include <map>
#include <vector>

struct InvertedIndex_entries {
  int doc_id;
  int pos_count;
  std::vector<int> ntoken;
  InvertedIndex_entries() : doc_id(0), pos_count(0) {
  }
  InvertedIndex_entries(int d, int p, std::vector<int>& tk)
      : doc_id(d), pos_count(p), ntoken(tk) {
  }
};
struct InvertedIndex {
  int doc_count;
  std::vector<InvertedIndex_entries> entries;
  InvertedIndex() : doc_count(0) {
  }
  InvertedIndex(int d, std::vector<InvertedIndex_entries>& e)
      : doc_count(d), entries(e) {
  }
};

struct ParsedDocument {
  std::vector<str> tags;
  std::vector<ParserResult> parsed;
};
using docmap = std::map<str, ParsedDocument>;
using indexmap = std::map<str, InvertedIndex>;

class IndexWriter {
 public:
  virtual void write(
      const docmap& csv,
      cstr& path,
      const int part_len,
      const int hash_len) const = 0;
  virtual void fill_docs(cstr& books_name) const = 0;
  // virtual void fill_entries(cstr& parsed) const = 0;
};

class TextIndexWriter : public IndexWriter {
 protected:
  int a, b;

 public:
  void write(
      const docmap& csv,
      cstr& path,
      const int part_len,
      const int hash_len) const override;
  void fill_docs(cstr& books_name) const override;
  // void fill_entries(cstr& parsed) const override;
};

class IndexBuilder {
 private:
  Parser p;
  std::vector<std::pair<str, short>> book_tags;
  int part_length;
  int hash_length;

 public:
  docmap loaded_document;

  IndexBuilder();
  IndexBuilder(cstr& books_name, cstr& config_name);

  indexmap build_inverted_index();

  int get_part_length() const {
    return part_length;
  }
  int get_hash_length() const {
    return hash_length;
  }

  bool add_document(str& line);
  bool read_index_properties(cstr& config_name);
  bool check_eq_tags(cstr& line, short pos) const;

  void print_index_properties() const;
  void print_documents() const;
  void print_results() const;
};

void create_folder(cstr& name);