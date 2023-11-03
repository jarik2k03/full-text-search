#pragma once
#include <commons/parser.h>

#include <picosha2.h>

#include <time.h>
#include <unistd.h>
#include <chrono>
#include <fstream>
#include <map>
#include <vector>

struct InvertedIndex_ {
  str doc_id;
  int pos_count;
  std::vector<int> ntoken;
  InvertedIndex_() : pos_count(0) {
  }
  InvertedIndex_(cstr& d, int p, std::vector<int>& tk)
      : doc_id(d), pos_count(p), ntoken(tk) {
  }
  void print_format() const {
    std::cout << " " << doc_id << " " << pos_count;
    for (auto i : ntoken)
      std::cout << " " << i;
  }
};
struct InvertedIndex {
  int doc_count;
  std::vector<InvertedIndex_> entries;

  InvertedIndex() : doc_count(0) {
  }
  InvertedIndex(int d, std::vector<InvertedIndex_>& e)
      : doc_count(d), entries(e) {
  }
  void print_format() const {
    std::cout << " " << doc_count;
    for (auto i : entries)
      i.print_format();
  }
};

struct ParsedDocument {
  std::vector<str> tags;
  std::vector<ParserResult> parsed;
};

using prvector = std::vector<str, ParserResult>;
using docmap = std::map<str, ParsedDocument>; // ключ - docID
using indexmap = std::map<str, InvertedIndex>; // ключ - ngram

struct IndexerResult {
  std::vector<indexmap> full_index;
  void traverse() const {
    for (auto& i : full_index)
      for (auto& idx : i) {
        std::cout << "\n" << idx.first << " ";
        idx.second.print_format();
      }
  };
};

class IndexWriter {
 public:
  virtual void write(cstr& path) const = 0;
  virtual void write_one(
      cstr& ngram,
      const InvertedIndex& cur,
      std::ofstream& file) const = 0;
  // virtual void fill_docs(cstr& books_name) const = 0;
  // virtual void fill_entries(cstr& parsed) const = 0;
};

class TextIndexWriter : public IndexWriter {
 protected:
  docmap& docindex;
  IndexerResult& indexresult;
  std::vector<std::pair<str, short>>& book_tags;

  int part_length;
  int hash_length;

 public:
  TextIndexWriter(
      docmap& dm,
      IndexerResult& ir,
      std::vector<std::pair<str, short>>& bm,
      const int p_l = 2,
      const int h_l = 6);
  void write(cstr& path) const override;
  void write_one(cstr& ngram, const InvertedIndex& cur, std::ofstream& file)
      const override;
  str name_to_hash(cstr& name) const;
  // void fill_entries(cstr& parsed) const override;
};

class IndexBuilder {
 private:
  Parser p;

  int part_length;
  int hash_length;

 public:
  docmap loaded_document;
  std::vector<std::pair<str, short>> book_tags;

  IndexBuilder();
  IndexBuilder(cstr& books_name, cstr& config_name);

  void add_for_ngram(indexmap& imap, cstr& ngram, const int row) const;
  IndexerResult build_inverted_index();

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