#pragma once
#ifndef SEARCHER_H
#define SEARCHER_H

#include <commons/_indexstructs.h>
#include <commons/parser.h>

#include <picosha2.h>

#include <time.h>
#include <unistd.h>
#include <cmath>
#include <filesystem>
#include <fstream>

class IndexAccessor {
 public:
  virtual ~IndexAccessor() = default;
  virtual invertedmap access_inverted(
      const ParserResult& ngrams,
      cstr& attr_name) const noexcept = 0;
  virtual scoreforwardumap access_forward(
      const invertedmap& im) const noexcept = 0;
  virtual scoreumap access_id(const invertedmap& im) const noexcept = 0;
  virtual forwardIndex read_forward(std::ifstream& fin) const noexcept = 0;
  virtual InvertedIndex read_inverted(cstr& line) const noexcept = 0;
};

class TextIndexAccessor : public IndexAccessor {
 private:
  IdxWriterOpts w_opts;
  IdxBuilderOpts b_opts;
  ParserOpts p_opts;
  str index_path;
  str name_to_hash(cstr& name) const noexcept;

 public:
  TextIndexAccessor(const Configurator& c, cstr& ip);
  invertedmap access_inverted(const ParserResult& ngrams, cstr& attr_name)
      const noexcept override;
  scoreforwardumap access_forward(
      const invertedmap& im) const noexcept override;
  scoreumap access_id(const invertedmap& im) const noexcept override;
  forwardIndex read_forward(std::ifstream& fin) const noexcept override;
  InvertedIndex read_inverted(cstr& line) const noexcept override;

  str& get_index_path() {
    return index_path;
  }
  ParserOpts& get_p_opts() {
    return p_opts;
  }
};

class IndexProcessor {
 private:
  std::map<str, str> search_attrs;
  scoreforwardumap doc_list;
  Parser parser;
  int all_docs;

  void summ_score(scoreumap& doc_ids);
  scoredocs sort_results() const noexcept;
  bool access_all_docs_dat(cstr& idx_path) noexcept;

 public:
  void calc_score(const ParserResult& pr, const invertedmap& entries);
  void calc_score(
      const ParserResult& pr,
      scoreumap& docs,
      const invertedmap& entries);
  IndexProcessor(const ParserOpts& po);
  IndexProcessor(const ParserOpts& po, const SearchState& s);
  void add_attribute(cstr& attr);
  int remove_attribute(cstr& attr);
  std::map<str, str>& get_attributes();
  void print_results(const scoredocs& res, cint row, cint ncols = 20)
      const noexcept;
  scoredocs search(TextIndexAccessor& access, str& title_request);

  int get_all_docs() const {
    return all_docs;
  }
  void set_all_docs(int d) {
    all_docs = d;
  }
  void set_doc_list(scoreforwardumap& d) {
    doc_list = d;
  }
  Parser get_parser() const {
    return parser;
  }
};

uint tf(cstr& term, int document_id, InvertedIndex& cur);
uint df(cstr& term, InvertedIndex& cur);
#endif