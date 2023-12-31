#pragma once

#ifndef INDEXSTRUCTS_H
#define INDEXSTRUCTS_H

#include <commons/_abstractions.h>
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
        std::cout << " " << +i;
      std::cout << '|';
    }
  };
  int doc_count;
  std::map<int, Entries> map;

  InvertedIndex() : doc_count(0) {
  }
  InvertedIndex(int d, std::map<int, Entries> e) : doc_count(d), map(e) {
  }
  void print_format() const noexcept {
    std::cout << " " << doc_count;
    for (auto i : map)
      i.second.print_format();
  }
};

using forwardIndex = std::vector<str>;
struct scoredoc {
  forwardIndex document;
  double score;
  scoredoc(forwardIndex d, double s) : document(d), score(s) {
  }
};
using forwardmap = std::map<str, forwardIndex>; // ключ - docID
using scoremap = std::map<uint, double>; // ключ - docID
using scoreforwardmap = std::map<uint, scoredoc>; // ключ - docID
using scoredocs =
    std::vector<std::pair<uint, scoredoc>>; // расширенный forwardmap
using invertedmap = std::map<str, InvertedIndex>; // ключ - ngram
using booktagsvector = std::vector<std::pair<str, short>>;
using forwardmaps = std::vector<forwardmap>;
using invertedmaps = std::vector<invertedmap>;
using IndexerResult = std::pair<forwardmap, invertedmaps>;

#endif