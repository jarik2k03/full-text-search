#pragma once
#ifndef INDEXER_H
#define INDEXER_H

#include <commons/_indexstructs.h>
#include <commons/parser.h>

#include <picosha2.h>

#include <time.h>
#include <unistd.h>
#include <filesystem>
#include <fstream>

class IndexAccessor {
 private:
  virtual InvertedIndex access_inverted(cstr& ngram) const noexcept = 0;
  virtual forwardmap access_all_forward(int id) const noexcept = 0;

 public:
  virtual ~IndexAccessor() = default;
};

class TextIndexAccessor : public IndexAccessor {
 private:
  IdxWriterOpts w_opts;
  ParserOpts p_opts;
  str index_path;

  InvertedIndex access_inverted(cstr& ngram) const noexcept override;
  forwardmap access_all_forward(int id) const noexcept override;
  str name_to_hash(cstr& name) const noexcept;

 public:
  TextIndexAccessor(const ParserOpts& po, const IdxWriterOpts& iwo, cstr& ip);

  str& get_index_path() {
    return index_path;
  }
};

class IndexProcessor {
 public:
  void search(cstr& request, const IndexAccessor* access);
};

#endif