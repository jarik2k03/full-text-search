#include <commons/parser.h>
#include <fstream>

#include <vector>

using ngrams_vec = std::vector<std::pair<ParserResult, int>>;

class IndexWriter {
 public:
  virtual void write(cstr& path) = 0;
};

class IndexBuilder {
 private:
  Parser p;
  ngrams_vec save_parsed;
  std::ifstream books;

 public:
  IndexBuilder();
  IndexBuilder(cstr& books_name, cstr& config_name, cstr& path = "user/");
  bool add_document(const int doc_id, str& text);

  void print_results() const;
};

class TextIndexWriter : public IndexWriter {
 protected:
  int a, b;

 public:
  void write(cstr& path);
};
