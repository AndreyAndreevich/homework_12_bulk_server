#include "Parser.h"

#include <stdexcept>

int start_parsing(int argc, char *argv[], uint n) {
  if (argc <= n) {
    throw std::runtime_error("The parameter is missing");
  }
  auto N = std::atoi(argv[n]);
  if (N <= 0 || std::string(argv[n]) != std::to_string(N)) {
    throw std::runtime_error("Incorrect parameter");
  }
  return N;
}

BlockParser::Block BlockParser::parsing(const std::string& line) {
  if (line.size() == 0) 
    return Block::Command;

  if (line == "{") {
    blocks_count++;
    if (blocks_count > 1) 
      return Block::Empty;
    else {
      is_block = true;
      return Block::StartBlock;
    }
  }

  if(line == "}") {
    if (blocks_count == 0) 
      return Block::Command;

    blocks_count--;
    if (blocks_count != 0) 
      return Block::Empty;
    else {
      is_block = false;
      return Block::CancelBlock;
    }
  }
  return Block::Command;
}