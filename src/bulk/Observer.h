#ifndef observer_h
#define observer_h

#include "Handler.h"

class Observer : public std::enable_shared_from_this<Observer> {
protected:
  using Commands = std::vector<std::string>;
  std::weak_ptr<Commands> _commands;
public:
  void subscribe(const std::weak_ptr<Handler>& handler) {
    if (!handler.expired())
      handler.lock()->subscribe(shared_from_this());
  }

  virtual void update(const std::weak_ptr<Commands>& commands) {
    _commands = commands;
  }

  virtual void print() = 0;
};

#endif