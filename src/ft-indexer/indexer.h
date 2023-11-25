#pragma once
#ifndef INDEXER_H
#define INDEXER_H

#include <commons/_indexstructs.h>
#include <commons/parser.h>

#include <picosha2.h>

#include <time.h>
#include <unistd.h>
#include <chrono>
#include <filesystem>
#include <fstream>

class IndexWriter {
 private:
  virtual void write_forward(const forwardIndex& data, std::ofstream& file)
      const noexcept = 0;
  virtual void write_inverted(
      cstr& ngram,
      const InvertedIndex& cur,
      std::ofstream& file) const noexcept = 0;

 public:
  virtual ~IndexWriter() = default;
  virtual void write_all_forward(const forwardmap& docindex, cstr& path)
      const noexcept = 0;
  virtual void write_all_inverted(const invertedmaps& indexresult, cstr& path)
      const noexcept = 0;
};

class TextIndexWriter : public IndexWriter {
 private:
  void write_forward(const forwardIndex& data, std::ofstream& file)
      const noexcept override;
  void write_inverted(
      cstr& ngram,
      const InvertedIndex& cur,
      std::ofstream& file) const noexcept override;

 protected:
  IdxWriterOpts options;
  booktagsvector book_tags;

 public:
  TextIndexWriter(const booktagsvector& bt, cint p_l = 3, cint h_l = 6);
  TextIndexWriter(const booktagsvector& bt, const IdxWriterOpts& iwo);

  void write_all_forward(const forwardmap& docindex, cstr& path)
      const noexcept override;
  void write_all_inverted(const invertedmaps& indexresult, cstr& path)
      const noexcept override;

  str name_to_hash(cstr& name) const noexcept;
};

class IndexBuilder {
 private:
  Parser parser;
  booktagsvector book_tags;
  invertedmaps build_all_inverted(const forwardmap& external) noexcept;
  forwardmap build_all_forward(cstr& book_name) noexcept;
  forwardmap build_all_forward(std::vector<str>& ram_book) noexcept;
  bool build_forward(forwardmap& doc, str& line) noexcept;
  void build_inverted(
      invertedmap& imap,
      const std::pair<cstr, uint8_t>& ng,
      const int row,
      const int cur_id) const noexcept;

 public:
  IndexBuilder(const booktagsvector& bt);
  IndexBuilder(const booktagsvector& bt, const ParserOpts& po);

  IndexerResult build_all(cstr& book_name) noexcept;
  IndexerResult build_all(std::vector<str>& ram_book) noexcept;

  bool set_data_from_config(const pugi::xml_document& d) noexcept;
  bool check_eq_tags(cstr& line, short pos) const noexcept;

  void print_index_properties() const noexcept;
  void print_documents(const forwardmap& fi) const noexcept;

  booktagsvector get_book_tags() const {
    return book_tags;
  }
};

void create_folder(cstr& name);

#endif