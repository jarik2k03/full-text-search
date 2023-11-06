#ifndef INDEXER_H
#define INDEXER_H

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
  InvertedIndex_(int p, std::vector<uint8_t> tks) : pos_count(p), ntoken(tks) {
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
  InvertedIndex(int d, std::map<int, InvertedIndex_> e) : doc_count(d), map(e) {
  }
  void print_format() const {
    std::cout << " " << doc_count;
    for (auto i : map)
      i.second.print_format();
  }
};
using CommonIndex = std::vector<str>;
using commonmap = std::map<str, CommonIndex>; // ключ - docID
using invertedmap = std::map<str, InvertedIndex>; // ключ - ngram
using booktagsvector = std::vector<std::pair<str, short>>;

struct InvertedResult {
  std::vector<invertedmap> full_index;
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
  virtual bool set_data(const pugi::xml_document& d) = 0;
  virtual void write_common(const commonmap& docindex, cstr& path) const = 0;
  virtual void write_inverted(const InvertedResult& indexresult, cstr& path)
      const = 0;
  virtual void write_one_common(const CommonIndex& data, std::ofstream& file)
      const = 0;
  virtual void write_one_inverted(
      cstr& ngram,
      const InvertedIndex& cur,
      std::ofstream& file) const = 0;
};

class TextIndexWriter : public IndexWriter {
 protected:
  booktagsvector book_tags;
  int part_length;
  int hash_length;

 public:
  TextIndexWriter(const booktagsvector& bt, cint p_l = 3, cint h_l = 6);
  TextIndexWriter(const pugi::xml_document& d);

  bool set_data(const pugi::xml_document& d) override;
  void write_common(const commonmap& docindex, cstr& path) const override;
  void write_inverted(const InvertedResult& indexresult, cstr& path)
      const override;
  void write_one_common(const CommonIndex& data, std::ofstream& file)
      const override;
  void write_one_inverted(
      cstr& ngram,
      const InvertedIndex& cur,
      std::ofstream& file) const override;
  str name_to_hash(cstr& name) const;

  int get_part_length() const {
    return part_length;
  }
  int get_hash_length() const {
    return hash_length;
  }
};

class IndexBuilder {
 private:
  Parser p;

 public:
  commonmap loaded_document;
  booktagsvector book_tags;
  IndexBuilder();
  IndexBuilder(const booktagsvector& bt);
  IndexBuilder(const pugi::xml_document& d);
  IndexBuilder(const commonmap& ld, const booktagsvector& bt);
  IndexBuilder(const pugi::xml_document& d, cstr& books_name);

  void add_one_inverted(
      invertedmap& imap,
      const std::pair<cstr, uint8_t>& ng,
      const int row,
      const int cur_id) const;
  InvertedResult build_inverted();
  InvertedResult build_inverted(const commonmap& external);
  commonmap build_common(cstr& book_name);
  commonmap build_common(std::vector<str>& ram_book);
  bool add_one_common(commonmap& doc, str& line);
  bool set_data(const pugi::xml_document& d);
  bool check_eq_tags(cstr& line, short pos) const;

  void print_index_properties() const;
  void print_documents() const;
};

void create_folder(cstr& name);

#endif