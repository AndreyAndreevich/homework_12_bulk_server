#ifndef handler_h
#define handler_h

#include <string>
#include <vector>
#include <memory>

#include "Parser.h"

class Observer;

class Handler {
  using Commands = std::vector<std::string>;

  std::vector<std::weak_ptr<Observer>> writers;
  std::shared_ptr<Commands> commands;
  int N = 0;
  uint max_size_commad = 50;

  void print();
  void update();
public:
  Handler(const int& n);
  void subscribe(const std::weak_ptr<Observer>& obs);
  void addCommand(const std::string& command);
  void stop();
};

#endif