#include "Handler.h"
#include "Observer.h"

#include <algorithm>

Handler::Handler(const int& n) {
  if (n <= 0) {
    throw std::runtime_error("error set N"); 
  } 
  N = n;
  commands = std::make_shared<Commands>();
}

void Handler::print() {
  for(auto& writer : writers) {
    if (!writer.expired()) {
      writer.lock()->print();
    }
  }
}

void Handler::update() {
  for(auto& writer : writers) {
    if (!writer.expired()) {
      writer.lock()->update(commands);
    }
  }
}

void Handler::subscribe(const std::weak_ptr<Observer>& obs) {
  writers.push_back(obs);
}

void Handler::addCommand(const std::string& command) { 
  if (command.size() > max_size_commad) {
    throw std::runtime_error("very large string");
  }

  commands->push_back(command);
  update();

  if (commands->size() == N) {
    print();
    commands->clear();
  }
}

void Handler::stop() {
  if (commands->size())
    print();
  commands->clear();
}
