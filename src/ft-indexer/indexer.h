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
  int pos_count;
  std::vector<uint8_t> ntoken;
  InvertedIndex_() : pos_count(0) {
  }
  InvertedIndex_(int p, std::vector<uint8_t>& tks) : pos_count(p), ntoken(tks) {
  }
  // самый частый конструктор
  InvertedIndex_(uint8_t tk) : pos_count(1) {
    ntoken.emplace_back(tk);
  }
  void print_format() const {
    std::cout << " " << pos_count;
    for (auto i : ntoken)
      std::cout << " " << i;
  }
};
struct InvertedIndex {
  int doc_count;
  std::map<int, InvertedIndex_> map;

  InvertedIndex() : doc_count(0) {
  }
  InvertedIndex(int d, std::map<int, InvertedIndex_>& e)
      : doc_count(d), map(e) {
  }
  void print_format() const {
    std::cout << " " << doc_count;
    for (auto i : map)
      i.second.print_format();
  }
};
using CommonIndex = std::vector<str>;

using docmap = std::map<str, CommonIndex>; // ключ - docID
using indexmap = std::map<str, InvertedIndex>; // ключ - ngram

using booktagsvector = std::vector<std::pair<str, short>>;

struct InvertedResult {
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
  virtual void write_common(cstr& path) const = 0;
  virtual void write_inverted(cstr& path) const = 0;
  virtual void write_one_common(const CommonIndex& data, std::ofstream& file)
      const = 0;
  virtual void write_one_inverted(
      cstr& ngram,
      const InvertedIndex& cur,
      std::ofstream& file) const = 0;
};

class TextIndexWriter : public IndexWriter {
 protected:
  docmap& docindex;
  InvertedResult& indexresult;
  booktagsvector& book_tags;

  int part_length;
  int hash_length;

 public:
  TextIndexWriter(
      docmap& dm,
      InvertedResult& ir,
      booktagsvector& bm,
      const int p_l = 2,
      const int h_l = 6);
  void write_common(cstr& path) const override;
  void write_inverted(cstr& path) const override;
  void write_one_common(const CommonIndex& data, std::ofstream& file)
      const override;
  void write_one_inverted(
      cstr& ngram,
      const InvertedIndex& cur,
      std::ofstream& file) const override;
  str name_to_hash(cstr& name) const;
};

class IndexBuilder {
 private:
  Parser p;
  int part_length;
  int hash_length;

 public:
  docmap loaded_document;
  booktagsvector book_tags;

  IndexBuilder(docmap& ld, booktagsvector& bt, int p_l = 2, int h_l = 6);
  IndexBuilder(const booktagsvector& bt, int p_l = 2, int h_l = 6);
  IndexBuilder(cstr& books_name, cstr& config_name);

  void add_one_inverted(
      indexmap& imap,
      const std::pair<cstr, uint8_t>& ng,
      const int row,
      const int cur_id) const;
  InvertedResult build_inverted();

  int get_part_length() const {
    return part_length;
  }
  int get_hash_length() const {
    return hash_length;
  }

  bool add_one_common(str& line);
  bool read_index_properties(cstr& config_name);
  bool check_eq_tags(cstr& line, short pos) const;

  void print_index_properties() const;
  void print_documents() const;
};

void create_folder(cstr& name);