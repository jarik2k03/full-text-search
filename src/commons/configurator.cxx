#include "configurator.h"

Configurator::Configurator(cstr& path_to_config) {
  ASSERT(
      !userconfig.load_file(path_to_config.c_str()),
      "Не найдена конфигурация fts");

  const pugi::xml_node ngram = userconfig.child("fts").child("ngram");
  parser.min_length = ngram.attribute("min").as_uint();
  parser.max_length = ngram.attribute("max").as_uint();
  const pugi::xml_node stop_words =
      userconfig.child("fts").child("ngram").child("stop_words");
  for (pugi::xml_node i : stop_words.children("word")) {
    parser.stopwords.insert(i.text().as_string());
  }

  const pugi::xml_node indexer = userconfig.child("fts").child("indexer");
  int count = 0;
  for (pugi::xml_node i : indexer.children("field")) {
    auto ignore = i.attribute("ignore");
    ++count;
    cstr val = ignore.value();
    short c = (val == "true") ? -1 : count;
    builder.words.emplace_back(i.text().as_string(), c);
  }
  writer.part_length = indexer.attribute("part_length").as_int();
  writer.hash_length = indexer.attribute("hash_length").as_int();
}

Configurator::Configurator() {
}

SearchState Configurator::get_request_from_doc(cstr& filename) {
  SearchState options;
  pugi::xml_document doc;
  doc.load_file(filename.c_str());
  const pugi::xml_node searcher = doc.child("searcher");
  options._title_request = searcher.child("request").text().as_string();
  for (pugi::xml_node subreq : searcher.children("subrequest")) {
    cstr attribute = subreq.attribute("attribute").as_string();
    cstr request = subreq.text().as_string();
    options._search_attrs.insert({attribute, request});
  }
  return options;
}