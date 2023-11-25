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