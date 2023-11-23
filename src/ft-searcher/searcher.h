#pragma once
#ifndef SEARCHER_H
#define SEARCHER_H

#include <commons/_indexstructs.h>
#include <commons/parser.h>

#include <picosha2.h>

#include <time.h>
#include <unistd.h>
#include <filesystem>
#include <fstream>
#include <set>

using searched = std::vector<std::pair<InvertedIndex, forwardmap>>;

class IndexAccessor {
 public:
  virtual ~IndexAccessor() = default;
  virtual InvertedIndex access_inverted(cstr& ngram, cstr& attr_name) const noexcept = 0;
  virtual forwardmap access_forward(InvertedIndex& ii) const noexcept = 0;
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
  TextIndexAccessor(
      const ParserOpts& po,
      const IdxWriterOpts& iwo,
      const IdxBuilderOpts& ibo,
      cstr& ip);
  InvertedIndex access_inverted(cstr& ngram, cstr& attr_name) const noexcept override;
  forwardmap access_forward(InvertedIndex& ii) const noexcept override;
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
  Parser parser;
  int all_docs;

 public:
  IndexProcessor(const ParserOpts& po);
  void add_attribute(cstr& attr);
  int remove_attribute(cstr& attr);
  std::map<str,str>& get_attributes();
  bool access_all_docs_dat(cstr& idx_path) noexcept;
  void search(TextIndexAccessor& access);
  int get_all_docs() const {
    return all_docs;
  }
};



uint tf(cstr& term, int document_id, InvertedIndex& cur);
uint df(cstr& term, InvertedIndex& cur);
#endif