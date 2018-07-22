#ifndef storage_h
#define storage_h

#include "Handler.h"
#include <map>
#include <vector>

class Storage {
  using buffer = std::string;
  using id = int;
  using handler = std::shared_ptr<Handler>;
  using writers = std::vector<std::shared_ptr<Observer>>;
  using element = std::tuple<handler,writers,buffer>;
  
  id count = 0;  

  Storage(Storage&) = delete;
  Storage& operator=(Storage&) = delete;

protected:
  Storage() = default;
  std::map<id,element> data;

public:
  static Storage& Instance();  
  id addHandler(const handler&, const writers&);
  element& getHandler(const id&);
  void removeHandler(const id&);
  void clear();
};

#endif