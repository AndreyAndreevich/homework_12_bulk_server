#ifndef writers_h
#define writers_h

#include <sstream>
#include <fstream>
#include <ctime>
#include <iostream>

#include "Observer.h"

class ConsoleWriter : public Observer {
  std::ostream* out;
public:
  ConsoleWriter(std::ostream& out_stream = std::cout);
  void setOutStream(std::ostream& out_stream);
  void print() override;
};

//---------------------------------------------------------------------------------

class FileWriter : public Observer {
  std::ofstream file;
  std::time_t time;
  std::string name;
  int section = 0;
  static int id_count;
  int id = 0;
  std::shared_ptr<std::vector<std::string>> name_list;
public:
  FileWriter();
  void setNameList(const std::shared_ptr<std::vector<std::string>>&);
  void update(const std::weak_ptr<Commands>& commands) override;
  void print() override;
  std::string getName();
  std::time_t getTime();
};

#endif