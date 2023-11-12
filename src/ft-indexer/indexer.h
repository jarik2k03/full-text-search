#pragma once
#ifndef INDEXER_H
#define INDEXER_H

#include <commons/parser.h>

#include <picosha2.h>

#include <time.h>
#include <unistd.h>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <map>
#include <vector>

struct InvertedIndex {
  struct Entries {
    int pos_count;
    std::vector<uint8_t> ntoken;
    Entries() : pos_count(0) {
    }
    Entries(int p, std::vector<uint8_t> tks) : pos_count(p), ntoken(tks) {
    }
    // самый частый конструктор
    Entries(uint8_t tk) : pos_count(1) {
      ntoken.emplace_back(tk);
    }
    void print_format() const {
      std::cout << " " << pos_count;
      for (auto i : ntoken)
        std::cout << " " << i;
    }
  };
  int doc_count;
  std::map<int, Entries> map;

  InvertedIndex() : doc_count(0) {
  }
  InvertedIndex(int d, std::map<int, Entries> e) : doc_count(d), map(e) {
  }
  void print_format() const {
    std::cout << " " << doc_count;
    for (auto i : map)
      i.second.print_format();
  }
};

using forwardIndex = std::vector<str>;
using forwardmap = std::map<str, forwardIndex>; // ключ - docID
using invertedmap = std::map<str, InvertedIndex>; // ключ - ngram
using booktagsvector = std::vector<std::pair<str, short>>;
struct invertedmaps {
  std::vector<invertedmap> full_index;
  void traverse() const {
    for (auto& i : full_index)
      for (auto& idx : i) {
        std::cout << "\n" << idx.first << " ";
        idx.second.print_format();
      }
  };
};
using IndexerResult = std::pair<forwardmap, invertedmaps>;


class IndexWriter {
 private:
  virtual void write_forward(const forwardIndex& data, std::ofstream& file)
      const = 0;
  virtual void write_inverted(
      cstr& ngram,
      const InvertedIndex& cur,
      std::ofstream& file) const = 0;

 public:
  virtual bool set_data_from_config(const pugi::xml_document& d) = 0;
  virtual void write_all_forward(const forwardmap& docindex, cstr& path)
      const = 0;
  virtual void write_all_inverted(const invertedmaps& indexresult, cstr& path)
      const = 0;
};

class TextIndexWriter : public IndexWriter {
 private:
  void write_forward(const forwardIndex& data, std::ofstream& file)
      const override;
  void write_inverted(
      cstr& ngram,
      const InvertedIndex& cur,
      std::ofstream& file) const override;

 protected:
  booktagsvector book_tags;
  int part_length;
  int hash_length;

 public:
  TextIndexWriter(const booktagsvector& bt, cint p_l = 3, cint h_l = 6);
  TextIndexWriter(const pugi::xml_document& d);

  bool set_data_from_config(const pugi::xml_document& d) override;
  void write_all_forward(const forwardmap& docindex, cstr& path) const override;
  void write_all_inverted(const invertedmaps& indexresult, cstr& path)
      const override;

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
  Parser parser;
  booktagsvector book_tags;
  invertedmaps build_all_inverted(const forwardmap& external);
  forwardmap build_all_forward(cstr& book_name);
  forwardmap build_all_forward(std::vector<str>& ram_book);
  bool build_forward(forwardmap& doc, str& line);
  void build_inverted(
      invertedmap& imap,
      const std::pair<cstr, uint8_t>& ng,
      const int row,
      const int cur_id) const;

 public:
  IndexBuilder(const booktagsvector& bt);
  IndexBuilder(const pugi::xml_document& d);

  IndexerResult build_all(cstr& book_name) noexcept;
  IndexerResult build_all(std::vector<str>& ram_book) noexcept;


  bool set_data_from_config(const pugi::xml_document& d);
  bool check_eq_tags(cstr& line, short pos) const;

  void print_index_properties() const;
  void print_documents(const forwardmap& fi) const;

  booktagsvector get_book_tags() const {
    return book_tags;
  }
};

void create_folder(cstr& name);

#endif