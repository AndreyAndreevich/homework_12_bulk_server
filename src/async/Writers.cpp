#include "Writers.h"

#include <iostream>
#include <thread>
  
ConsoleWriter::ConsoleWriter(std::ostream& out_stream) {
  setOutStream(out_stream);
}

void ConsoleWriter::setOutStream(std::ostream& out_stream) {
  out = &out_stream;
}

void ConsoleWriter::print() {
  if (_commands.expired()) {
    throw std::runtime_error("commands do not exist");
  }
  auto commands = _commands.lock();
  *out << "bulk: ";
  for(auto command = commands->cbegin(); command < commands->cend(); command++) {
    if (command != commands->cbegin())
      *out << ", ";
    *out << *command;
  }
  *out << std::endl;
}

//---------------------------------------------------------------------------------

FileWriter::FileWriter() {
  id_count++;
  id = id_count;
  file.exceptions ( std::ifstream::failbit | std::ifstream::badbit );
}

int FileWriter::id_count = 0;

void FileWriter::setNameList(const std::shared_ptr<std::vector<std::string>>& name_list_) {
  name_list = name_list_;
}

void FileWriter::update(const std::weak_ptr<Commands>& commands) {
  if (commands.expired()) {
    throw std::runtime_error("commands do not exist");
  }
  if (commands.lock()->size() == 1) {
    std::stringbuf out_buffer;
    std::ostream out_stream(&out_buffer);
    auto time_ = time;
    auto current_time = std::time(&time);
    if (time_ == current_time) {
      section++;
    } else {
      section = 0;
    }
    out_stream << "bulk_" << section << "_";
    out_stream << id << "_";
    out_stream << current_time << ".log";
    name = out_buffer.str();
  }
  Observer::update(commands);
} 

#include <iostream>

void FileWriter::print() {
  if (_commands.expired()) {
    throw std::runtime_error("commands do not exist");
  }
  auto commands = _commands.lock();
  file.open(name);
  file << "bulk: ";
  for(auto command = commands->cbegin(); command < commands->cend(); command++) {
    if (command != commands->cbegin())
      file << ", ";
    file << *command;
  }
  file.close();
  if (name_list) {
    name_list->push_back(name);
  }
}

std::string FileWriter::getName() {
  return name;
}

std::time_t FileWriter::getTime() {
  return time;
}