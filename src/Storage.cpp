#include "Storage.h"
#include "Handler.h"

Storage& Storage::Instance() {
  static Storage instance;
  return instance;
}

Storage::id Storage::addHandler(const handler& handler_, const writers& writers_) {
  count++;
  data.emplace_hint(data.end(),count,
    std::make_tuple(handler_,writers_,std::string{}));
  return count;
}

Storage::element& Storage::getHandler(const id& id_) {
  return data.at(id_);
}

void Storage::removeHandler(const id& id_) {
  data.erase(id_);
}

void Storage::clear() {
  data.clear();
  count = 0;
}